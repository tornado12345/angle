//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// ShaderGL.h: Defines the class interface for ShaderGL.

#ifndef LIBANGLE_RENDERER_GL_SHADERGL_H_
#define LIBANGLE_RENDERER_GL_SHADERGL_H_

#include "libANGLE/renderer/ShaderImpl.h"

namespace rx
{
class RendererGL;
struct WorkaroundsGL;
enum class MultiviewImplementationTypeGL;

class ShaderGL : public ShaderImpl
{
  public:
    ShaderGL(const gl::ShaderState &data,
             GLuint shaderID,
             MultiviewImplementationTypeGL multiviewImplementationType,
             const std::shared_ptr<RendererGL> &renderer);
    ~ShaderGL() override;

    void destroy() override;

    // ShaderImpl implementation
    ShCompileOptions prepareSourceAndReturnOptions(const gl::Context *context,
                                                   std::stringstream *sourceStream,
                                                   std::string *sourcePath) override;
    void compileAsync(const std::string &source, std::string &infoLog) override;
    bool postTranslateCompile(gl::ShCompilerInstance *compiler, std::string *infoLog) override;
    std::string getDebugInfo() const override;

    GLuint getShaderID() const;

  private:
    void compileAndCheckShader(const char *source);

    GLuint mShaderID;
    MultiviewImplementationTypeGL mMultiviewImplementationType;
    std::shared_ptr<RendererGL> mRenderer;
    bool mFallbackToMainThread;
    GLint mCompileStatus;
    std::string mInfoLog;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GL_SHADERGL_H_
