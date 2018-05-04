// GENERATED FILE - DO NOT EDIT.
// Generated by generate_entry_points.py using data from gl.xml.
//
// Copyright 2018 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// entry_points_gles_1_0_autogen.h:
//   Defines the GLES 1.0 entry points.

#ifndef LIBGLESV2_ENTRY_POINTS_GLES_1_0_AUTOGEN_H_
#define LIBGLESV2_ENTRY_POINTS_GLES_1_0_AUTOGEN_H_

#include <GLES/gl.h>
#include <export.h>

namespace gl
{
ANGLE_EXPORT void GL_APIENTRY AlphaFunc(GLenum func, GLfloat ref);
ANGLE_EXPORT void GL_APIENTRY AlphaFuncx(GLenum func, GLfixed ref);
ANGLE_EXPORT void GL_APIENTRY ClearColorx(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
ANGLE_EXPORT void GL_APIENTRY ClearDepthx(GLfixed depth);
ANGLE_EXPORT void GL_APIENTRY ClientActiveTexture(GLenum texture);
ANGLE_EXPORT void GL_APIENTRY ClipPlanef(GLenum p, const GLfloat *eqn);
ANGLE_EXPORT void GL_APIENTRY ClipPlanex(GLenum plane, const GLfixed *equation);
ANGLE_EXPORT void GL_APIENTRY Color4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
ANGLE_EXPORT void GL_APIENTRY Color4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
ANGLE_EXPORT void GL_APIENTRY Color4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
ANGLE_EXPORT void GL_APIENTRY ColorPointer(GLint size,
                                           GLenum type,
                                           GLsizei stride,
                                           const void *pointer);
ANGLE_EXPORT void GL_APIENTRY DepthRangex(GLfixed n, GLfixed f);
ANGLE_EXPORT void GL_APIENTRY DisableClientState(GLenum array);
ANGLE_EXPORT void GL_APIENTRY EnableClientState(GLenum array);
ANGLE_EXPORT void GL_APIENTRY Fogf(GLenum pname, GLfloat param);
ANGLE_EXPORT void GL_APIENTRY Fogfv(GLenum pname, const GLfloat *params);
ANGLE_EXPORT void GL_APIENTRY Fogx(GLenum pname, GLfixed param);
ANGLE_EXPORT void GL_APIENTRY Fogxv(GLenum pname, const GLfixed *param);
ANGLE_EXPORT void GL_APIENTRY
Frustumf(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);
ANGLE_EXPORT void GL_APIENTRY
Frustumx(GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f);
ANGLE_EXPORT void GL_APIENTRY GetClipPlanef(GLenum plane, GLfloat *equation);
ANGLE_EXPORT void GL_APIENTRY GetClipPlanex(GLenum plane, GLfixed *equation);
ANGLE_EXPORT void GL_APIENTRY GetFixedv(GLenum pname, GLfixed *params);
ANGLE_EXPORT void GL_APIENTRY GetLightfv(GLenum light, GLenum pname, GLfloat *params);
ANGLE_EXPORT void GL_APIENTRY GetLightxv(GLenum light, GLenum pname, GLfixed *params);
ANGLE_EXPORT void GL_APIENTRY GetMaterialfv(GLenum face, GLenum pname, GLfloat *params);
ANGLE_EXPORT void GL_APIENTRY GetMaterialxv(GLenum face, GLenum pname, GLfixed *params);
ANGLE_EXPORT void GL_APIENTRY GetPointerv(GLenum pname, void **params);
ANGLE_EXPORT void GL_APIENTRY GetTexEnvfv(GLenum target, GLenum pname, GLfloat *params);
ANGLE_EXPORT void GL_APIENTRY GetTexEnviv(GLenum target, GLenum pname, GLint *params);
ANGLE_EXPORT void GL_APIENTRY GetTexEnvxv(GLenum target, GLenum pname, GLfixed *params);
ANGLE_EXPORT void GL_APIENTRY GetTexParameterxv(GLenum target, GLenum pname, GLfixed *params);
ANGLE_EXPORT void GL_APIENTRY LightModelf(GLenum pname, GLfloat param);
ANGLE_EXPORT void GL_APIENTRY LightModelfv(GLenum pname, const GLfloat *params);
ANGLE_EXPORT void GL_APIENTRY LightModelx(GLenum pname, GLfixed param);
ANGLE_EXPORT void GL_APIENTRY LightModelxv(GLenum pname, const GLfixed *param);
ANGLE_EXPORT void GL_APIENTRY Lightf(GLenum light, GLenum pname, GLfloat param);
ANGLE_EXPORT void GL_APIENTRY Lightfv(GLenum light, GLenum pname, const GLfloat *params);
ANGLE_EXPORT void GL_APIENTRY Lightx(GLenum light, GLenum pname, GLfixed param);
ANGLE_EXPORT void GL_APIENTRY Lightxv(GLenum light, GLenum pname, const GLfixed *params);
ANGLE_EXPORT void GL_APIENTRY LineWidthx(GLfixed width);
ANGLE_EXPORT void GL_APIENTRY LoadIdentity();
ANGLE_EXPORT void GL_APIENTRY LoadMatrixf(const GLfloat *m);
ANGLE_EXPORT void GL_APIENTRY LoadMatrixx(const GLfixed *m);
ANGLE_EXPORT void GL_APIENTRY LogicOp(GLenum opcode);
ANGLE_EXPORT void GL_APIENTRY Materialf(GLenum face, GLenum pname, GLfloat param);
ANGLE_EXPORT void GL_APIENTRY Materialfv(GLenum face, GLenum pname, const GLfloat *params);
ANGLE_EXPORT void GL_APIENTRY Materialx(GLenum face, GLenum pname, GLfixed param);
ANGLE_EXPORT void GL_APIENTRY Materialxv(GLenum face, GLenum pname, const GLfixed *param);
ANGLE_EXPORT void GL_APIENTRY MatrixMode(GLenum mode);
ANGLE_EXPORT void GL_APIENTRY MultMatrixf(const GLfloat *m);
ANGLE_EXPORT void GL_APIENTRY MultMatrixx(const GLfixed *m);
ANGLE_EXPORT void GL_APIENTRY
MultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
ANGLE_EXPORT void GL_APIENTRY
MultiTexCoord4x(GLenum texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
ANGLE_EXPORT void GL_APIENTRY Normal3f(GLfloat nx, GLfloat ny, GLfloat nz);
ANGLE_EXPORT void GL_APIENTRY Normal3x(GLfixed nx, GLfixed ny, GLfixed nz);
ANGLE_EXPORT void GL_APIENTRY NormalPointer(GLenum type, GLsizei stride, const void *pointer);
ANGLE_EXPORT void GL_APIENTRY
Orthof(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);
ANGLE_EXPORT void GL_APIENTRY
Orthox(GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f);
ANGLE_EXPORT void GL_APIENTRY PointParameterf(GLenum pname, GLfloat param);
ANGLE_EXPORT void GL_APIENTRY PointParameterfv(GLenum pname, const GLfloat *params);
ANGLE_EXPORT void GL_APIENTRY PointParameterx(GLenum pname, GLfixed param);
ANGLE_EXPORT void GL_APIENTRY PointParameterxv(GLenum pname, const GLfixed *params);
ANGLE_EXPORT void GL_APIENTRY PointSize(GLfloat size);
ANGLE_EXPORT void GL_APIENTRY PointSizex(GLfixed size);
ANGLE_EXPORT void GL_APIENTRY PolygonOffsetx(GLfixed factor, GLfixed units);
ANGLE_EXPORT void GL_APIENTRY PopMatrix();
ANGLE_EXPORT void GL_APIENTRY PushMatrix();
ANGLE_EXPORT void GL_APIENTRY Rotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
ANGLE_EXPORT void GL_APIENTRY Rotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
ANGLE_EXPORT void GL_APIENTRY SampleCoveragex(GLclampx value, GLboolean invert);
ANGLE_EXPORT void GL_APIENTRY Scalef(GLfloat x, GLfloat y, GLfloat z);
ANGLE_EXPORT void GL_APIENTRY Scalex(GLfixed x, GLfixed y, GLfixed z);
ANGLE_EXPORT void GL_APIENTRY ShadeModel(GLenum mode);
ANGLE_EXPORT void GL_APIENTRY TexCoordPointer(GLint size,
                                              GLenum type,
                                              GLsizei stride,
                                              const void *pointer);
ANGLE_EXPORT void GL_APIENTRY TexEnvf(GLenum target, GLenum pname, GLfloat param);
ANGLE_EXPORT void GL_APIENTRY TexEnvfv(GLenum target, GLenum pname, const GLfloat *params);
ANGLE_EXPORT void GL_APIENTRY TexEnvi(GLenum target, GLenum pname, GLint param);
ANGLE_EXPORT void GL_APIENTRY TexEnviv(GLenum target, GLenum pname, const GLint *params);
ANGLE_EXPORT void GL_APIENTRY TexEnvx(GLenum target, GLenum pname, GLfixed param);
ANGLE_EXPORT void GL_APIENTRY TexEnvxv(GLenum target, GLenum pname, const GLfixed *params);
ANGLE_EXPORT void GL_APIENTRY TexParameterx(GLenum target, GLenum pname, GLfixed param);
ANGLE_EXPORT void GL_APIENTRY TexParameterxv(GLenum target, GLenum pname, const GLfixed *params);
ANGLE_EXPORT void GL_APIENTRY Translatef(GLfloat x, GLfloat y, GLfloat z);
ANGLE_EXPORT void GL_APIENTRY Translatex(GLfixed x, GLfixed y, GLfixed z);
ANGLE_EXPORT void GL_APIENTRY VertexPointer(GLint size,
                                            GLenum type,
                                            GLsizei stride,
                                            const void *pointer);
}  // namespace gl

#endif  // LIBGLESV2_ENTRY_POINTS_GLES_1_0_AUTOGEN_H_
