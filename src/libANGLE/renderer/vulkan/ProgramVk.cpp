//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ProgramVk.cpp:
//    Implements the class methods for ProgramVk.
//

#include "libANGLE/renderer/vulkan/ProgramVk.h"

#include "common/debug.h"
#include "common/utilities.h"
#include "libANGLE/Context.h"
#include "libANGLE/renderer/vulkan/ContextVk.h"
#include "libANGLE/renderer/vulkan/GlslangWrapper.h"
#include "libANGLE/renderer/vulkan/RendererVk.h"

namespace rx
{

namespace
{

gl::Error InitDefaultUniformBlock(const gl::Context *context,
                                  VkDevice device,
                                  gl::Shader *shader,
                                  vk::BufferAndMemory *storageOut,
                                  sh::BlockLayoutMap *blockLayoutMapOut,
                                  size_t *requiredSizeOut)
{
    const auto &uniforms = shader->getUniforms(context);

    if (uniforms.empty())
    {
        *requiredSizeOut = 0;
        return gl::NoError();
    }

    sh::Std140BlockEncoder blockEncoder;
    sh::GetUniformBlockInfo(uniforms, "", &blockEncoder, false, blockLayoutMapOut);

    size_t blockSize = blockEncoder.getBlockSize();

    // TODO(jmadill): I think we still need a valid block for the pipeline even if zero sized.
    if (blockSize == 0)
    {
        *requiredSizeOut = 0;
        return gl::NoError();
    }

    VkBufferCreateInfo uniformBufferInfo;
    uniformBufferInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    uniformBufferInfo.pNext                 = nullptr;
    uniformBufferInfo.flags                 = 0;
    uniformBufferInfo.size                  = blockSize;
    uniformBufferInfo.usage                 = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    uniformBufferInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    uniformBufferInfo.queueFamilyIndexCount = 0;
    uniformBufferInfo.pQueueFamilyIndices   = nullptr;

    ANGLE_TRY(storageOut->buffer.init(device, uniformBufferInfo));

    ANGLE_TRY(AllocateBufferMemory(GetImplAs<ContextVk>(context), blockSize, &storageOut->buffer,
                                   &storageOut->memory, requiredSizeOut));

    return gl::NoError();
}

template <typename T>
void UpdateDefaultUniformBlock(GLsizei count,
                               int componentCount,
                               const T *v,
                               const sh::BlockMemberInfo &layoutInfo,
                               angle::MemoryBuffer *uniformData)
{
    // Assume an offset of -1 means the block is unused.
    if (layoutInfo.offset == -1)
    {
        return;
    }

    int elementSize = sizeof(T) * componentCount;
    if (layoutInfo.arrayStride == 0 || layoutInfo.arrayStride == elementSize)
    {
        uint8_t *writePtr = uniformData->data() + layoutInfo.offset;
        memcpy(writePtr, v, elementSize * count);
    }
    else
    {
        UNIMPLEMENTED();
    }
}

vk::Error SyncDefaultUniformBlock(VkDevice device,
                                  vk::DeviceMemory *bufferMemory,
                                  const angle::MemoryBuffer &bufferData)
{
    ASSERT(bufferMemory->valid() && !bufferData.empty());
    uint8_t *mapPointer = nullptr;
    ANGLE_TRY(bufferMemory->map(device, 0, bufferData.size(), 0, &mapPointer));
    memcpy(mapPointer, bufferData.data(), bufferData.size());
    bufferMemory->unmap(device);
    return vk::NoError();
}

enum ShaderIndex : uint32_t
{
    MinShaderIndex = 0,
    VertexShader   = MinShaderIndex,
    FragmentShader = 1,
    MaxShaderIndex = 2,
};

gl::Shader *GetShader(const gl::ProgramState &programState, uint32_t shaderIndex)
{
    switch (shaderIndex)
    {
        case VertexShader:
            return programState.getAttachedVertexShader();
        case FragmentShader:
            return programState.getAttachedFragmentShader();
        default:
            UNREACHABLE();
            return nullptr;
    }
}

}  // anonymous namespace

ProgramVk::DefaultUniformBlock::DefaultUniformBlock()
    : storage(), uniformData(), uniformsDirty(false), uniformLayout()
{
}

ProgramVk::ProgramVk(const gl::ProgramState &state)
    : ProgramImpl(state), mDefaultUniformBlocks(), mDescriptorSet(VK_NULL_HANDLE)
{
}

ProgramVk::~ProgramVk()
{
}

void ProgramVk::destroy(const gl::Context *contextImpl)
{
    VkDevice device = GetImplAs<ContextVk>(contextImpl)->getDevice();
    reset(device);
}

void ProgramVk::reset(VkDevice device)
{
    for (auto &uniformBlock : mDefaultUniformBlocks)
    {
        uniformBlock.storage.memory.destroy(device);
        uniformBlock.storage.buffer.destroy(device);
    }

    mEmptyUniformBlockStorage.memory.destroy(device);
    mEmptyUniformBlockStorage.buffer.destroy(device);

    for (auto &descriptorSetLayout : mDescriptorSetLayouts)
    {
        descriptorSetLayout.destroy(device);
    }

    mLinkedFragmentModule.destroy(device);
    mLinkedVertexModule.destroy(device);
    mPipelineLayout.destroy(device);

    // Descriptor Sets are pool allocated, so do not need to be explicitly freed.
    mDescriptorSet = VK_NULL_HANDLE;
}

gl::LinkResult ProgramVk::load(const gl::Context *contextImpl,
                               gl::InfoLog &infoLog,
                               gl::BinaryInputStream *stream)
{
    UNIMPLEMENTED();
    return gl::InternalError();
}

void ProgramVk::save(const gl::Context *context, gl::BinaryOutputStream *stream)
{
    UNIMPLEMENTED();
}

void ProgramVk::setBinaryRetrievableHint(bool retrievable)
{
    UNIMPLEMENTED();
}

void ProgramVk::setSeparable(bool separable)
{
    UNIMPLEMENTED();
}

gl::LinkResult ProgramVk::link(const gl::Context *glContext,
                               const gl::VaryingPacking &packing,
                               gl::InfoLog &infoLog)
{
    ContextVk *contextVk           = GetImplAs<ContextVk>(glContext);
    RendererVk *renderer           = contextVk->getRenderer();
    GlslangWrapper *glslangWrapper = renderer->getGlslangWrapper();
    VkDevice device                = renderer->getDevice();

    reset(device);

    std::vector<uint32_t> vertexCode;
    std::vector<uint32_t> fragmentCode;
    bool linkSuccess = false;
    ANGLE_TRY_RESULT(glslangWrapper->linkProgram(glContext, mState, &vertexCode, &fragmentCode),
                     linkSuccess);
    if (!linkSuccess)
    {
        return false;
    }

    {
        VkShaderModuleCreateInfo vertexShaderInfo;
        vertexShaderInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertexShaderInfo.pNext    = nullptr;
        vertexShaderInfo.flags    = 0;
        vertexShaderInfo.codeSize = vertexCode.size() * sizeof(uint32_t);
        vertexShaderInfo.pCode    = vertexCode.data();

        ANGLE_TRY(mLinkedVertexModule.init(device, vertexShaderInfo));
    }

    {
        VkShaderModuleCreateInfo fragmentShaderInfo;
        fragmentShaderInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragmentShaderInfo.pNext    = nullptr;
        fragmentShaderInfo.flags    = 0;
        fragmentShaderInfo.codeSize = fragmentCode.size() * sizeof(uint32_t);
        fragmentShaderInfo.pCode    = fragmentCode.data();

        ANGLE_TRY(mLinkedFragmentModule.init(device, fragmentShaderInfo));
    }

    ANGLE_TRY(initPipelineLayout(contextVk));
    ANGLE_TRY(initDescriptorSets(contextVk));
    ANGLE_TRY(initDefaultUniformBlocks(glContext));

    return true;
}

gl::Error ProgramVk::initDefaultUniformBlocks(const gl::Context *glContext)
{
    ContextVk *contextVk = GetImplAs<ContextVk>(glContext);
    VkDevice device      = contextVk->getDevice();

    // Process vertex and fragment uniforms into std140 packing.
    std::array<sh::BlockLayoutMap, 2> layoutMap;
    std::array<size_t, 2> requiredBufferSize = {{0, 0}};

    for (uint32_t shaderIndex = MinShaderIndex; shaderIndex < MaxShaderIndex; ++shaderIndex)
    {
        ANGLE_TRY(InitDefaultUniformBlock(glContext, device, GetShader(mState, shaderIndex),
                                          &mDefaultUniformBlocks[shaderIndex].storage,
                                          &layoutMap[shaderIndex],
                                          &requiredBufferSize[shaderIndex]));
    }

    // Init the default block layout info.
    const auto &locations = mState.getUniformLocations();
    const auto &uniforms  = mState.getUniforms();
    for (size_t locationIndex = 0; locationIndex < locations.size(); ++locationIndex)
    {
        std::array<sh::BlockMemberInfo, 2> layoutInfo;

        const auto &location = locations[locationIndex];
        if (location.used() && !location.ignored)
        {
            const auto &uniform     = uniforms[location.index];
            std::string uniformName = uniform.name;
            if (uniform.isArray())
            {
                uniformName += ArrayIndexString(location.arrayIndices);
            }

            bool found = false;

            for (uint32_t shaderIndex = MinShaderIndex; shaderIndex < MaxShaderIndex; ++shaderIndex)
            {
                auto it = layoutMap[shaderIndex].find(uniformName);
                if (it != layoutMap[shaderIndex].end())
                {
                    found                   = true;
                    layoutInfo[shaderIndex] = it->second;
                }
            }

            ASSERT(found);
        }

        for (uint32_t shaderIndex = MinShaderIndex; shaderIndex < MaxShaderIndex; ++shaderIndex)
        {
            mDefaultUniformBlocks[shaderIndex].uniformLayout.push_back(layoutInfo[shaderIndex]);
        }
    }

    bool anyDirty = false;
    bool allDirty = true;

    for (uint32_t shaderIndex = MinShaderIndex; shaderIndex < MaxShaderIndex; ++shaderIndex)
    {
        if (requiredBufferSize[shaderIndex] > 0)
        {
            if (!mDefaultUniformBlocks[shaderIndex].uniformData.resize(
                    requiredBufferSize[shaderIndex]))
            {
                return gl::OutOfMemory() << "Memory allocation failure.";
            }
            mDefaultUniformBlocks[shaderIndex].uniformData.fill(0);
            mDefaultUniformBlocks[shaderIndex].uniformsDirty = true;

            anyDirty = true;
        }
        else
        {
            allDirty = false;
        }
    }

    if (anyDirty)
    {
        // Initialize the "empty" uniform block if necessary.
        if (!allDirty)
        {
            VkBufferCreateInfo uniformBufferInfo;
            uniformBufferInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            uniformBufferInfo.pNext                 = nullptr;
            uniformBufferInfo.flags                 = 0;
            uniformBufferInfo.size                  = 1;
            uniformBufferInfo.usage                 = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            uniformBufferInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
            uniformBufferInfo.queueFamilyIndexCount = 0;
            uniformBufferInfo.pQueueFamilyIndices   = nullptr;

            ANGLE_TRY(mEmptyUniformBlockStorage.buffer.init(device, uniformBufferInfo));

            size_t requiredSize = 0;
            ANGLE_TRY(AllocateBufferMemory(contextVk, 1, &mEmptyUniformBlockStorage.buffer,
                                           &mEmptyUniformBlockStorage.memory, &requiredSize));
        }

        ANGLE_TRY(updateDefaultUniformsDescriptorSet(contextVk));
    }

    return gl::NoError();
}

GLboolean ProgramVk::validate(const gl::Caps &caps, gl::InfoLog *infoLog)
{
    UNIMPLEMENTED();
    return GLboolean();
}

template <typename T>
void ProgramVk::setUniformImpl(GLint location, GLsizei count, const T *v, GLenum entryPointType)
{
    const gl::VariableLocation &locationInfo = mState.getUniformLocations()[location];
    const gl::LinkedUniform &linkedUniform   = mState.getUniforms()[locationInfo.index];

    if (linkedUniform.type == entryPointType)
    {
        for (auto &uniformBlock : mDefaultUniformBlocks)
        {
            const sh::BlockMemberInfo &layoutInfo = uniformBlock.uniformLayout[location];
            UpdateDefaultUniformBlock(count, linkedUniform.typeInfo->componentCount, v, layoutInfo,
                                      &uniformBlock.uniformData);
        }
    }
    else
    {
        ASSERT(linkedUniform.type == gl::VariableBoolVectorType(entryPointType));
        UNIMPLEMENTED();
    }
}

void ProgramVk::setUniform1fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT);
}

void ProgramVk::setUniform2fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT_VEC2);
}

void ProgramVk::setUniform3fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT_VEC3);
}

void ProgramVk::setUniform4fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT_VEC4);
}

void ProgramVk::setUniform1iv(GLint location, GLsizei count, const GLint *v)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniform2iv(GLint location, GLsizei count, const GLint *v)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniform3iv(GLint location, GLsizei count, const GLint *v)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniform4iv(GLint location, GLsizei count, const GLint *v)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniform1uiv(GLint location, GLsizei count, const GLuint *v)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniform2uiv(GLint location, GLsizei count, const GLuint *v)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniform3uiv(GLint location, GLsizei count, const GLuint *v)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniform4uiv(GLint location, GLsizei count, const GLuint *v)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniformMatrix2fv(GLint location,
                                    GLsizei count,
                                    GLboolean transpose,
                                    const GLfloat *value)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniformMatrix3fv(GLint location,
                                    GLsizei count,
                                    GLboolean transpose,
                                    const GLfloat *value)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniformMatrix4fv(GLint location,
                                    GLsizei count,
                                    GLboolean transpose,
                                    const GLfloat *value)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniformMatrix2x3fv(GLint location,
                                      GLsizei count,
                                      GLboolean transpose,
                                      const GLfloat *value)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniformMatrix3x2fv(GLint location,
                                      GLsizei count,
                                      GLboolean transpose,
                                      const GLfloat *value)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniformMatrix2x4fv(GLint location,
                                      GLsizei count,
                                      GLboolean transpose,
                                      const GLfloat *value)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniformMatrix4x2fv(GLint location,
                                      GLsizei count,
                                      GLboolean transpose,
                                      const GLfloat *value)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniformMatrix3x4fv(GLint location,
                                      GLsizei count,
                                      GLboolean transpose,
                                      const GLfloat *value)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniformMatrix4x3fv(GLint location,
                                      GLsizei count,
                                      GLboolean transpose,
                                      const GLfloat *value)
{
    UNIMPLEMENTED();
}

void ProgramVk::setUniformBlockBinding(GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    UNIMPLEMENTED();
}

bool ProgramVk::getUniformBlockSize(const std::string &blockName,
                                    const std::string &blockMappedName,
                                    size_t *sizeOut) const
{
    UNIMPLEMENTED();
    return bool();
}

bool ProgramVk::getUniformBlockMemberInfo(const std::string &memberUniformName,
                                          const std::string &memberUniformMappedName,
                                          sh::BlockMemberInfo *memberInfoOut) const
{
    UNIMPLEMENTED();
    return bool();
}

void ProgramVk::setPathFragmentInputGen(const std::string &inputName,
                                        GLenum genMode,
                                        GLint components,
                                        const GLfloat *coeffs)
{
    UNIMPLEMENTED();
}

const vk::ShaderModule &ProgramVk::getLinkedVertexModule() const
{
    ASSERT(mLinkedVertexModule.getHandle() != VK_NULL_HANDLE);
    return mLinkedVertexModule;
}

const vk::ShaderModule &ProgramVk::getLinkedFragmentModule() const
{
    ASSERT(mLinkedFragmentModule.getHandle() != VK_NULL_HANDLE);
    return mLinkedFragmentModule;
}

const vk::PipelineLayout &ProgramVk::getPipelineLayout() const
{
    return mPipelineLayout;
}

vk::Error ProgramVk::initPipelineLayout(ContextVk *context)
{
    ASSERT(!mPipelineLayout.valid());

    VkDevice device = context->getDevice();

    // Create two descriptor set layouts: one for default uniform info, and one for textures.
    // Skip one or both if there are no uniforms.
    VkDescriptorSetLayoutBinding uniformBindings[2];
    uint32_t blockCount = 0;

    {
        auto &layoutBinding = uniformBindings[blockCount];

        layoutBinding.binding            = blockCount;
        layoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBinding.descriptorCount    = 1;
        layoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
        layoutBinding.pImmutableSamplers = nullptr;

        blockCount++;
    }

    {
        auto &layoutBinding = uniformBindings[blockCount];

        layoutBinding.binding            = blockCount;
        layoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBinding.descriptorCount    = 1;
        layoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
        layoutBinding.pImmutableSamplers = nullptr;

        blockCount++;
    }

    {
        VkDescriptorSetLayoutCreateInfo uniformInfo;
        uniformInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        uniformInfo.pNext        = nullptr;
        uniformInfo.flags        = 0;
        uniformInfo.bindingCount = blockCount;
        uniformInfo.pBindings    = uniformBindings;

        vk::DescriptorSetLayout uniformLayout;
        ANGLE_TRY(uniformLayout.init(device, uniformInfo));
        mDescriptorSetLayouts.push_back(std::move(uniformLayout));
    }

    VkPipelineLayoutCreateInfo createInfo;
    createInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext                  = nullptr;
    createInfo.flags                  = 0;
    createInfo.setLayoutCount         = static_cast<uint32_t>(mDescriptorSetLayouts.size());
    createInfo.pSetLayouts            = mDescriptorSetLayouts[0].ptr();
    createInfo.pushConstantRangeCount = 0;
    createInfo.pPushConstantRanges    = nullptr;

    ANGLE_TRY(mPipelineLayout.init(device, createInfo));

    return vk::NoError();
}

vk::Error ProgramVk::initDescriptorSets(ContextVk *contextVk)
{
    ASSERT(mDescriptorSet == VK_NULL_HANDLE);

    VkDevice device = contextVk->getDevice();

    // Write out to a new a descriptor set.
    // TODO(jmadill): Handle descriptor set lifetime.
    vk::DescriptorPool *descriptorPool = contextVk->getDescriptorPool();

    VkDescriptorSetAllocateInfo allocInfo;
    allocInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext          = nullptr;
    allocInfo.descriptorPool = descriptorPool->getHandle();

    // TODO(jmadill): Handle descriptor set layouts for textures.
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts        = mDescriptorSetLayouts[0].ptr();

    ANGLE_TRY(descriptorPool->allocateDescriptorSets(device, allocInfo, &mDescriptorSet));
    return vk::NoError();
}

void ProgramVk::getUniformfv(const gl::Context *context, GLint location, GLfloat *params) const
{
    UNIMPLEMENTED();
}

void ProgramVk::getUniformiv(const gl::Context *context, GLint location, GLint *params) const
{
    UNIMPLEMENTED();
}

void ProgramVk::getUniformuiv(const gl::Context *context, GLint location, GLuint *params) const
{
    UNIMPLEMENTED();
}

vk::Error ProgramVk::updateUniforms(ContextVk *contextVk)
{
    if (!mDefaultUniformBlocks[VertexShader].uniformsDirty &&
        !mDefaultUniformBlocks[FragmentShader].uniformsDirty)
    {
        return vk::NoError();
    }

    VkDevice device = contextVk->getDevice();

    // Update buffer memory by immediate mapping. This immediate update only works once.
    // TODO(jmadill): Handle inserting updates into the command stream, or use dynamic buffers.
    for (auto &uniformBlock : mDefaultUniformBlocks)
    {
        if (uniformBlock.uniformsDirty)
        {
            ANGLE_TRY(SyncDefaultUniformBlock(device, &uniformBlock.storage.memory,
                                              uniformBlock.uniformData));
            uniformBlock.uniformsDirty = false;
        }
    }

    return vk::NoError();
}

vk::Error ProgramVk::updateDefaultUniformsDescriptorSet(ContextVk *contextVk)
{
    std::array<VkDescriptorBufferInfo, 2> descriptorBufferInfo;
    std::array<VkWriteDescriptorSet, 2> writeDescriptorInfo;
    uint32_t bufferCount = 0;

    for (auto &uniformBlock : mDefaultUniformBlocks)
    {
        auto &bufferInfo = descriptorBufferInfo[bufferCount];

        if (!uniformBlock.uniformData.empty())
        {
            bufferInfo.buffer = uniformBlock.storage.buffer.getHandle();
        }
        else
        {
            bufferInfo.buffer = mEmptyUniformBlockStorage.buffer.getHandle();
        }

        bufferInfo.offset = 0;
        bufferInfo.range  = VK_WHOLE_SIZE;

        auto &writeInfo = writeDescriptorInfo[bufferCount];

        writeInfo.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeInfo.pNext            = nullptr;
        writeInfo.dstSet           = mDescriptorSet;
        writeInfo.dstBinding       = bufferCount;
        writeInfo.dstArrayElement  = 0;
        writeInfo.descriptorCount  = 1;
        writeInfo.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeInfo.pImageInfo       = nullptr;
        writeInfo.pBufferInfo      = &bufferInfo;
        writeInfo.pTexelBufferView = nullptr;

        bufferCount++;
    }

    VkDevice device = contextVk->getDevice();

    vkUpdateDescriptorSets(device, bufferCount, writeDescriptorInfo.data(), 0, nullptr);

    return vk::NoError();
}

VkDescriptorSet ProgramVk::getDescriptorSet() const
{
    return mDescriptorSet;
}

}  // namespace rx
