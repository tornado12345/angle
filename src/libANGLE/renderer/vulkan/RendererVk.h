//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// RendererVk.h:
//    Defines the class interface for RendererVk.
//

#ifndef LIBANGLE_RENDERER_VULKAN_RENDERERVK_H_
#define LIBANGLE_RENDERER_VULKAN_RENDERERVK_H_

#include <memory>
#include <vulkan/vulkan.h>

#include "common/angleutils.h"
#include "libANGLE/Caps.h"
#include "libANGLE/renderer/vulkan/CommandGraph.h"
#include "libANGLE/renderer/vulkan/vk_format_utils.h"
#include "libANGLE/renderer/vulkan/vk_internal_shaders.h"

namespace egl
{
class AttributeMap;
}

namespace rx
{
class FramebufferVk;
class GlslangWrapper;

namespace vk
{
struct Format;
}

class RendererVk : angle::NonCopyable
{
  public:
    RendererVk();
    ~RendererVk();

    vk::Error initialize(const egl::AttributeMap &attribs, const char *wsiName);

    std::string getVendorString() const;
    std::string getRendererDescription() const;

    VkInstance getInstance() const { return mInstance; }
    VkPhysicalDevice getPhysicalDevice() const { return mPhysicalDevice; }
    const VkPhysicalDeviceProperties &getPhysicalDeviceProperties() const
    {
        return mPhysicalDeviceProperties;
    }
    VkQueue getQueue() const { return mQueue; }
    VkDevice getDevice() const { return mDevice; }

    vk::ErrorOrResult<uint32_t> selectPresentQueueForSurface(VkSurfaceKHR surface);

    vk::Error finish(const gl::Context *context);
    vk::Error flush(const gl::Context *context,
                    const vk::Semaphore &waitSemaphore,
                    const vk::Semaphore &signalSemaphore);

    const vk::CommandPool &getCommandPool() const;

    const gl::Caps &getNativeCaps() const;
    const gl::TextureCapsMap &getNativeTextureCaps() const;
    const gl::Extensions &getNativeExtensions() const;
    const gl::Limitations &getNativeLimitations() const;
    uint32_t getMaxActiveTextures();
    uint32_t getUniformBufferDescriptorCount();

    GlslangWrapper *getGlslangWrapper();

    Serial getCurrentQueueSerial() const;

    bool isResourceInUse(const vk::CommandGraphResource &resource);
    bool isSerialInUse(Serial serial);

    template <typename T>
    void releaseResource(const vk::CommandGraphResource &resource, T *object)
    {
        Serial resourceSerial = resource.getQueueSerial();
        releaseObject(resourceSerial, object);
    }

    template <typename T>
    void releaseObject(Serial resourceSerial, T *object)
    {
        if (!isSerialInUse(resourceSerial))
        {
            object->destroy(mDevice);
        }
        else
        {
            object->dumpResources(resourceSerial, &mGarbage);
        }
    }

    uint32_t getQueueFamilyIndex() const { return mCurrentQueueFamilyIndex; }

    const vk::MemoryProperties &getMemoryProperties() const { return mMemoryProperties; }

    // TODO(jmadill): We could pass angle::Format::ID here.
    const vk::Format &getFormat(GLenum internalFormat) const
    {
        return mFormatTable[internalFormat];
    }

    vk::Error getCompatibleRenderPass(const vk::RenderPassDesc &desc,
                                      vk::RenderPass **renderPassOut);
    vk::Error getRenderPassWithOps(const vk::RenderPassDesc &desc,
                                   const vk::AttachmentOpsArray &ops,
                                   vk::RenderPass **renderPassOut);

    // For getting a vk::Pipeline for the an application's draw call. RenderPassDesc is automatic.
    vk::Error getAppPipeline(const ProgramVk *programVk,
                             const vk::PipelineDesc &desc,
                             const gl::AttributesMask &activeAttribLocationsMask,
                             vk::PipelineAndSerial **pipelineOut);

    // For getting a vk::Pipeline for an internal draw call. Use an explicit RenderPass.
    vk::Error getInternalPipeline(const vk::ShaderAndSerial &vertexShader,
                                  const vk::ShaderAndSerial &fragmentShader,
                                  const vk::PipelineLayout &pipelineLayout,
                                  const vk::PipelineDesc &pipelineDesc,
                                  const gl::AttributesMask &activeAttribLocationsMask,
                                  vk::PipelineAndSerial **pipelineOut);

    // This should only be called from ResourceVk.
    // TODO(jmadill): Keep in ContextVk to enable threaded rendering.
    vk::CommandGraphNode *allocateCommandNode();

    const vk::PipelineLayout &getGraphicsPipelineLayout() const;
    const std::vector<vk::DescriptorSetLayout> &getGraphicsDescriptorSetLayouts() const;

    // Used in internal shaders.
    vk::Error getInternalUniformPipelineLayout(const vk::PipelineLayout **pipelineLayoutOut);
    const vk::DescriptorSetLayout &getInternalUniformDescriptorSetLayout() const;

    // Issues a new serial for linked shader modules. Used in the pipeline cache.
    Serial issueShaderSerial();

    vk::ShaderLibrary *getShaderLibrary();

  private:
    vk::Error initializeDevice(uint32_t queueFamilyIndex);
    void ensureCapsInitialized() const;
    vk::Error submitFrame(const VkSubmitInfo &submitInfo, vk::CommandBuffer &&commandBatch);
    vk::Error checkInFlightCommands();
    void freeAllInFlightResources();
    vk::Error flushCommandGraph(const gl::Context *context, vk::CommandBuffer *commandBatch);
    vk::Error initGraphicsPipelineLayout();

    mutable bool mCapsInitialized;
    mutable gl::Caps mNativeCaps;
    mutable gl::TextureCapsMap mNativeTextureCaps;
    mutable gl::Extensions mNativeExtensions;
    mutable gl::Limitations mNativeLimitations;

    VkInstance mInstance;
    bool mEnableValidationLayers;
    VkDebugReportCallbackEXT mDebugReportCallback;
    VkPhysicalDevice mPhysicalDevice;
    VkPhysicalDeviceProperties mPhysicalDeviceProperties;
    std::vector<VkQueueFamilyProperties> mQueueFamilyProperties;
    VkQueue mQueue;
    uint32_t mCurrentQueueFamilyIndex;
    VkDevice mDevice;
    vk::CommandPool mCommandPool;
    GlslangWrapper *mGlslangWrapper;
    SerialFactory mQueueSerialFactory;
    SerialFactory mShaderSerialFactory;
    Serial mLastCompletedQueueSerial;
    Serial mCurrentQueueSerial;

    struct CommandBatch final : angle::NonCopyable
    {
        CommandBatch();
        ~CommandBatch();
        CommandBatch(CommandBatch &&other);
        CommandBatch &operator=(CommandBatch &&other);

        vk::CommandPool commandPool;
        vk::Fence fence;
        Serial serial;
    };

    std::vector<CommandBatch> mInFlightCommands;
    std::vector<vk::GarbageObject> mGarbage;
    vk::MemoryProperties mMemoryProperties;
    vk::FormatTable mFormatTable;

    RenderPassCache mRenderPassCache;
    PipelineCache mPipelineCache;

    // See CommandGraph.h for a desription of the Command Graph.
    vk::CommandGraph mCommandGraph;

    // ANGLE uses a single pipeline layout for all GL programs. It is owned here in the Renderer.
    // See the design doc for an overview of the pipeline layout structure.
    vk::PipelineLayout mGraphicsPipelineLayout;
    std::vector<vk::DescriptorSetLayout> mGraphicsDescriptorSetLayouts;

    // Used for internal shaders.
    vk::PipelineLayout mInternalUniformPipelineLayout;

    // Internal shader library.
    vk::ShaderLibrary mShaderLibrary;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_VULKAN_RENDERERVK_H_
