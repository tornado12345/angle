// GENERATED FILE - DO NOT EDIT.
// Generated by gen_vk_internal_shaders.py using data from shaders/src/*
//
// Copyright 2018 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// vk_internal_shaders_autogen.h:
//   Pre-generated shader library for the ANGLE Vulkan back-end.

#ifndef LIBANGLE_RENDERER_VULKAN_VK_INTERNAL_SHADERS_AUTOGEN_H_
#define LIBANGLE_RENDERER_VULKAN_VK_INTERNAL_SHADERS_AUTOGEN_H_

#include <stddef.h>
#include <stdint.h>

#include <utility>

namespace rx
{
namespace vk
{
enum class InternalShaderID
{
    FullScreenQuad_vert,
    UniformColor_frag,
    EnumCount
};

namespace priv
{
// This is SPIR-V binary blob and the size.
struct ShaderBlob
{
    const uint32_t *code;
    size_t codeSize;
};
const ShaderBlob &GetInternalShaderBlob(InternalShaderID shaderID);
}  // priv
}  // namespace vk
}  // namespace rx

#endif  // LIBANGLE_RENDERER_VULKAN_VK_INTERNAL_SHADERS_AUTOGEN_H_
