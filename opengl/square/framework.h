
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _USE_MATH_DEFINES               // for M_PI

// Windows Header Files
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

// OpenGL headers
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glext.h>

/*
opengl32.lib
glu32.lib
*/

extern PFNGLGENQUERIESPROC                                  glGenQueries;
extern PFNGLDELETEQUERIESPROC                               glDeleteQueries;
extern PFNGLISQUERYPROC                                     glIsQuery;
extern PFNGLBEGINQUERYPROC                                  glBeginQuery;
extern PFNGLENDQUERYPROC                                    glEndQuery;
extern PFNGLGETQUERYIVPROC                                  glGetQueryiv;
extern PFNGLGETQUERYOBJECTIVPROC                            glGetQueryObjectiv;
extern PFNGLGETQUERYOBJECTUIVPROC                           glGetQueryObjectuiv;
extern PFNGLBINDBUFFERPROC                                  glBindBuffer;
extern PFNGLDELETEBUFFERSPROC                               glDeleteBuffers;
extern PFNGLGENBUFFERSPROC                                  glGenBuffers;
extern PFNGLISBUFFERPROC                                    glIsBuffer;
extern PFNGLBUFFERDATAPROC                                  glBufferData;
extern PFNGLBUFFERSUBDATAPROC                               glBufferSubData;
extern PFNGLGETBUFFERSUBDATAPROC                            glGetBufferSubData;
extern PFNGLMAPBUFFERPROC                                   glMapBuffer;
extern PFNGLUNMAPBUFFERPROC                                 glUnmapBuffer;
extern PFNGLGETBUFFERPARAMETERIVPROC                        glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC                           glGetBufferPointerv;
extern PFNGLBLENDEQUATIONSEPARATEPROC                       glBlendEquationSeparate;
extern PFNGLDRAWBUFFERSPROC                                 glDrawBuffers;
extern PFNGLSTENCILOPSEPARATEPROC                           glStencilOpSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC                         glStencilFuncSeparate;
extern PFNGLSTENCILMASKSEPARATEPROC                         glStencilMaskSeparate;
extern PFNGLATTACHSHADERPROC                                glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC                          glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC                               glCompileShader;
extern PFNGLCREATEPROGRAMPROC                               glCreateProgram;
extern PFNGLCREATESHADERPROC                                glCreateShader;
extern PFNGLDELETEPROGRAMPROC                               glDeleteProgram;
extern PFNGLDELETESHADERPROC                                glDeleteShader;
extern PFNGLDETACHSHADERPROC                                glDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC                    glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC                     glEnableVertexAttribArray;
extern PFNGLGETACTIVEATTRIBPROC                             glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC                            glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC                          glGetAttachedShaders;
extern PFNGLGETATTRIBLOCATIONPROC                           glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC                                glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC                           glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC                                 glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC                            glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC                             glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC                          glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC                                glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC                                glGetUniformiv;
extern PFNGLGETVERTEXATTRIBDVPROC                           glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVPROC                           glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVPROC                           glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC                     glGetVertexAttribPointerv;
extern PFNGLISPROGRAMPROC                                   glIsProgram;
extern PFNGLISSHADERPROC                                    glIsShader;
extern PFNGLLINKPROGRAMPROC                                 glLinkProgram;
extern PFNGLSHADERSOURCEPROC                                glShaderSource;
extern PFNGLUSEPROGRAMPROC                                  glUseProgram;
extern PFNGLUNIFORM1FPROC                                   glUniform1f;
extern PFNGLUNIFORM2FPROC                                   glUniform2f;
extern PFNGLUNIFORM3FPROC                                   glUniform3f;
extern PFNGLUNIFORM4FPROC                                   glUniform4f;
extern PFNGLUNIFORM1IPROC                                   glUniform1i;
extern PFNGLUNIFORM2IPROC                                   glUniform2i;
extern PFNGLUNIFORM3IPROC                                   glUniform3i;
extern PFNGLUNIFORM4IPROC                                   glUniform4i;
extern PFNGLUNIFORM1FVPROC                                  glUniform1fv;
extern PFNGLUNIFORM2FVPROC                                  glUniform2fv;
extern PFNGLUNIFORM3FVPROC                                  glUniform3fv;
extern PFNGLUNIFORM4FVPROC                                  glUniform4fv;
extern PFNGLUNIFORM1IVPROC                                  glUniform1iv;
extern PFNGLUNIFORM2IVPROC                                  glUniform2iv;
extern PFNGLUNIFORM3IVPROC                                  glUniform3iv;
extern PFNGLUNIFORM4IVPROC                                  glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC                            glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC                            glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC                            glUniformMatrix4fv;
extern PFNGLVALIDATEPROGRAMPROC                             glValidateProgram;
extern PFNGLVERTEXATTRIB1DPROC                              glVertexAttrib1d;
extern PFNGLVERTEXATTRIB1DVPROC                             glVertexAttrib1dv;
extern PFNGLVERTEXATTRIB1FPROC                              glVertexAttrib1f;
extern PFNGLVERTEXATTRIB1FVPROC                             glVertexAttrib1fv;
extern PFNGLVERTEXATTRIB1SPROC                              glVertexAttrib1s;
extern PFNGLVERTEXATTRIB1SVPROC                             glVertexAttrib1sv;
extern PFNGLVERTEXATTRIB2DPROC                              glVertexAttrib2d;
extern PFNGLVERTEXATTRIB2DVPROC                             glVertexAttrib2dv;
extern PFNGLVERTEXATTRIB2FPROC                              glVertexAttrib2f;
extern PFNGLVERTEXATTRIB2FVPROC                             glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB2SPROC                              glVertexAttrib2s;
extern PFNGLVERTEXATTRIB2SVPROC                             glVertexAttrib2sv;
extern PFNGLVERTEXATTRIB3DPROC                              glVertexAttrib3d;
extern PFNGLVERTEXATTRIB3DVPROC                             glVertexAttrib3dv;
extern PFNGLVERTEXATTRIB3FPROC                              glVertexAttrib3f;
extern PFNGLVERTEXATTRIB3FVPROC                             glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB3SPROC                              glVertexAttrib3s;
extern PFNGLVERTEXATTRIB3SVPROC                             glVertexAttrib3sv;
extern PFNGLVERTEXATTRIB4NBVPROC                            glVertexAttrib4Nbv;
extern PFNGLVERTEXATTRIB4NIVPROC                            glVertexAttrib4Niv;
extern PFNGLVERTEXATTRIB4NSVPROC                            glVertexAttrib4Nsv;
extern PFNGLVERTEXATTRIB4NUBPROC                            glVertexAttrib4Nub;
extern PFNGLVERTEXATTRIB4NUBVPROC                           glVertexAttrib4Nubv;
extern PFNGLVERTEXATTRIB4NUIVPROC                           glVertexAttrib4Nuiv;
extern PFNGLVERTEXATTRIB4NUSVPROC                           glVertexAttrib4Nusv;
extern PFNGLVERTEXATTRIB4BVPROC                             glVertexAttrib4bv;
extern PFNGLVERTEXATTRIB4DPROC                              glVertexAttrib4d;
extern PFNGLVERTEXATTRIB4DVPROC                             glVertexAttrib4dv;
extern PFNGLVERTEXATTRIB4FPROC                              glVertexAttrib4f;
extern PFNGLVERTEXATTRIB4FVPROC                             glVertexAttrib4fv;
extern PFNGLVERTEXATTRIB4IVPROC                             glVertexAttrib4iv;
extern PFNGLVERTEXATTRIB4SPROC                              glVertexAttrib4s;
extern PFNGLVERTEXATTRIB4SVPROC                             glVertexAttrib4sv;
extern PFNGLVERTEXATTRIB4UBVPROC                            glVertexAttrib4ubv;
extern PFNGLVERTEXATTRIB4UIVPROC                            glVertexAttrib4uiv;
extern PFNGLVERTEXATTRIB4USVPROC                            glVertexAttrib4usv;
extern PFNGLVERTEXATTRIBPOINTERPROC                         glVertexAttribPointer;
extern PFNGLCOLORMASKIPROC                                  glColorMaski;
extern PFNGLGETBOOLEANI_VPROC                               glGetBooleani_v;
extern PFNGLGETINTEGERI_VPROC                               glGetIntegeri_v;
extern PFNGLENABLEIPROC                                     glEnablei;
extern PFNGLDISABLEIPROC                                    glDisablei;
extern PFNGLISENABLEDIPROC                                  glIsEnabledi;
extern PFNGLBEGINTRANSFORMFEEDBACKPROC                      glBeginTransformFeedback;
extern PFNGLENDTRANSFORMFEEDBACKPROC                        glEndTransformFeedback;
extern PFNGLBINDBUFFERRANGEPROC                             glBindBufferRange;
extern PFNGLBINDBUFFERBASEPROC                              glBindBufferBase;
extern PFNGLTRANSFORMFEEDBACKVARYINGSPROC                   glTransformFeedbackVaryings;
extern PFNGLGETTRANSFORMFEEDBACKVARYINGPROC                 glGetTransformFeedbackVarying;
extern PFNGLCLAMPCOLORPROC                                  glClampColor;
extern PFNGLBEGINCONDITIONALRENDERPROC                      glBeginConditionalRender;
extern PFNGLENDCONDITIONALRENDERPROC                        glEndConditionalRender;
extern PFNGLVERTEXATTRIBIPOINTERPROC                        glVertexAttribIPointer;
extern PFNGLGETVERTEXATTRIBIIVPROC                          glGetVertexAttribIiv;
extern PFNGLGETVERTEXATTRIBIUIVPROC                         glGetVertexAttribIuiv;
extern PFNGLVERTEXATTRIBI1IPROC                             glVertexAttribI1i;
extern PFNGLVERTEXATTRIBI2IPROC                             glVertexAttribI2i;
extern PFNGLVERTEXATTRIBI3IPROC                             glVertexAttribI3i;
extern PFNGLVERTEXATTRIBI4IPROC                             glVertexAttribI4i;
extern PFNGLVERTEXATTRIBI1UIPROC                            glVertexAttribI1ui;
extern PFNGLVERTEXATTRIBI2UIPROC                            glVertexAttribI2ui;
extern PFNGLVERTEXATTRIBI3UIPROC                            glVertexAttribI3ui;
extern PFNGLVERTEXATTRIBI4UIPROC                            glVertexAttribI4ui;
extern PFNGLVERTEXATTRIBI1IVPROC                            glVertexAttribI1iv;
extern PFNGLVERTEXATTRIBI2IVPROC                            glVertexAttribI2iv;
extern PFNGLVERTEXATTRIBI3IVPROC                            glVertexAttribI3iv;
extern PFNGLVERTEXATTRIBI4IVPROC                            glVertexAttribI4iv;
extern PFNGLVERTEXATTRIBI1UIVPROC                           glVertexAttribI1uiv;
extern PFNGLVERTEXATTRIBI2UIVPROC                           glVertexAttribI2uiv;
extern PFNGLVERTEXATTRIBI3UIVPROC                           glVertexAttribI3uiv;
extern PFNGLVERTEXATTRIBI4UIVPROC                           glVertexAttribI4uiv;
extern PFNGLVERTEXATTRIBI4BVPROC                            glVertexAttribI4bv;
extern PFNGLVERTEXATTRIBI4SVPROC                            glVertexAttribI4sv;
extern PFNGLVERTEXATTRIBI4UBVPROC                           glVertexAttribI4ubv;
extern PFNGLVERTEXATTRIBI4USVPROC                           glVertexAttribI4usv;
extern PFNGLGETUNIFORMUIVPROC                               glGetUniformuiv;
extern PFNGLBINDFRAGDATALOCATIONPROC                        glBindFragDataLocation;
extern PFNGLGETFRAGDATALOCATIONPROC                         glGetFragDataLocation;
extern PFNGLUNIFORM1UIPROC                                  glUniform1ui;
extern PFNGLUNIFORM2UIPROC                                  glUniform2ui;
extern PFNGLUNIFORM3UIPROC                                  glUniform3ui;
extern PFNGLUNIFORM4UIPROC                                  glUniform4ui;
extern PFNGLUNIFORM1UIVPROC                                 glUniform1uiv;
extern PFNGLUNIFORM2UIVPROC                                 glUniform2uiv;
extern PFNGLUNIFORM3UIVPROC                                 glUniform3uiv;
extern PFNGLUNIFORM4UIVPROC                                 glUniform4uiv;
extern PFNGLTEXPARAMETERIIVPROC                             glTexParameterIiv;
extern PFNGLTEXPARAMETERIUIVPROC                            glTexParameterIuiv;
extern PFNGLGETTEXPARAMETERIIVPROC                          glGetTexParameterIiv;
extern PFNGLGETTEXPARAMETERIUIVPROC                         glGetTexParameterIuiv;
extern PFNGLCLEARBUFFERIVPROC                               glClearBufferiv;
extern PFNGLCLEARBUFFERUIVPROC                              glClearBufferuiv;
extern PFNGLCLEARBUFFERFVPROC                               glClearBufferfv;
extern PFNGLCLEARBUFFERFIPROC                               glClearBufferfi;
extern PFNGLGETSTRINGIPROC                                  glGetStringi;
extern PFNGLISRENDERBUFFERPROC                              glIsRenderbuffer;
extern PFNGLBINDRENDERBUFFERPROC                            glBindRenderbuffer;
extern PFNGLDELETERENDERBUFFERSPROC                         glDeleteRenderbuffers;
extern PFNGLGENRENDERBUFFERSPROC                            glGenRenderbuffers;
extern PFNGLRENDERBUFFERSTORAGEPROC                         glRenderbufferStorage;
extern PFNGLGETRENDERBUFFERPARAMETERIVPROC                  glGetRenderbufferParameteriv;
extern PFNGLISFRAMEBUFFERPROC                               glIsFramebuffer;
extern PFNGLBINDFRAMEBUFFERPROC                             glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC                          glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC                             glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC                      glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE1DPROC                        glFramebufferTexture1D;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC                        glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERTEXTURE3DPROC                        glFramebufferTexture3D;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC                     glFramebufferRenderbuffer;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC         glGetFramebufferAttachmentParameteriv;
extern PFNGLGENERATEMIPMAPPROC                              glGenerateMipmap;
extern PFNGLBLITFRAMEBUFFERPROC                             glBlitFramebuffer;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC              glRenderbufferStorageMultisample;
extern PFNGLFRAMEBUFFERTEXTURELAYERPROC                     glFramebufferTextureLayer;
extern PFNGLMAPBUFFERRANGEPROC                              glMapBufferRange;
extern PFNGLFLUSHMAPPEDBUFFERRANGEPROC                      glFlushMappedBufferRange;
extern PFNGLBINDVERTEXARRAYPROC                             glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC                          glDeleteVertexArrays;
extern PFNGLGENVERTEXARRAYSPROC                             glGenVertexArrays;
extern PFNGLISVERTEXARRAYPROC                               glIsVertexArray;

void LoadOpenGLFunctions();
bool FindExtension(const char* str);
void OutputExtenstions();

