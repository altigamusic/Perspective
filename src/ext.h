//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2015                                   //
//--------------------------------------------------------------------------//

#ifndef _EXTENSIONS_H_
#define _EXTENSIONS_H_

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include "glext.h"


#ifndef _DEBUG
#define NUMFUNCTIONS 19
#else
#define NUMFUNCTIONS 24
#endif

extern void* myglfunc[NUMFUNCTIONS];

#define glCreateShaderProgramv ((PFNGLCREATESHADERPROGRAMVPROC)myglfunc[0])
#define glGenProgramPipelines ((PFNGLGENPROGRAMPIPELINESPROC)myglfunc[1])
#define glBindProgramPipeline ((PFNGLBINDPROGRAMPIPELINEPROC)myglfunc[2])
#define glUseProgramStages ((PFNGLUSEPROGRAMSTAGESPROC)myglfunc[3])
#define glProgramUniform4fv ((PFNGLPROGRAMUNIFORM4FVPROC)myglfunc[4])
#define glGenFramebuffers ((PFNGLGENFRAMEBUFFERSPROC)myglfunc[5])
#define glBindFramebuffer ((PFNGLBINDFRAMEBUFFERPROC)myglfunc[6])
#define glFramebufferTexture2D ((PFNGLFRAMEBUFFERTEXTURE2DPROC)myglfunc[7])
#define glCreateProgram ((PFNGLCREATEPROGRAMPROC)myglfunc[8])
#define glAttachShader ((PFNGLATTACHSHADERPROC)myglfunc[9])
#define glLinkProgram ((PFNGLLINKPROGRAMPROC)myglfunc[10])
#define glCreateShader ((PFNGLCREATESHADERPROC)myglfunc[11])
#define glShaderSource ((PFNGLSHADERSOURCEPROC)myglfunc[12])
#define glCompileShader ((PFNGLCOMPILESHADERPROC)myglfunc[13])
#define glUseProgram ((PFNGLUSEPROGRAMPROC)myglfunc[14])
#define glActiveTexture ((PFNGLACTIVETEXTUREPROC)myglfunc[15])
#define glGetUniformLocation ((PFNGLGETUNIFORMLOCATIONPROC)myglfunc[16])
#define glUniform1i ((PFNGLUNIFORM1IPROC)myglfunc[17])
#define glUniform3f ((PFNGLUNIFORM3FPROC)myglfunc[18])
#define glUniform1f ((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))

#ifdef _DEBUG
#define glGetProgramiv          ((PFNGLGETPROGRAMIVPROC)myglfunc[19])
#define glGetProgramInfoLog     ((PFNGLGETPROGRAMINFOLOGPROC)myglfunc[20])
#define glCheckFramebufferStatus ((PFNGLCHECKFRAMEBUFFERSTATUSPROC)myglfunc[21])
#define glGetShaderiv          ((PFNGLGETSHADERIVPROC)myglfunc[22])
#define glGetShaderInfoLog     ((PFNGLGETSHADERINFOLOGPROC)myglfunc[23])

#define glBlitFramebuffer ((PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress("glBlitFramebuffer"))
#define glDeleteProgram ((PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram"))
#endif

// init
int EXT_Init(void);

#endif
