//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2015                                   //
//--------------------------------------------------------------------------//

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include "glext.h"
#ifdef LINUX
#include <GL/glx.h>
#endif

//--- d a t a ---------------------------------------------------------------

#include "ext.h"

static char* strs[] = {
    "glCreateShaderProgramv",
    "glGenProgramPipelines",
    "glBindProgramPipeline",
    "glUseProgramStages",
    "glProgramUniform4fv",
    "glGenFramebuffers",
    "glBindFramebuffer",
    "glFramebufferTexture2D",
    "glCreateProgram",
    "glAttachShader",
    "glLinkProgram",
    "glCreateShader",
    "glShaderSource",
    "glCompileShader",
    "glUseProgram",
    "glActiveTexture",
    "glGetUniformLocation",
    "glUniform1i",
    "glUniform3f",

#ifdef _DEBUG
    "glGetProgramiv",
    "glGetProgramInfoLog",
    "glCheckFramebufferStatus",
    "glGetShaderiv",
    "glGetShaderInfoLog",
#endif
};

void* myglfunc[NUMFUNCTIONS];

int EXT_Init(void)
{
	for (int i = 0; i < NUMFUNCTIONS; i++)
	{
#ifdef WINDOWS
		myglfunc[i] = wglGetProcAddress(strs[i]);
#endif
#ifdef LINUX
		myglfunc[i] = glXGetProcAddress((const unsigned char*)strs[i]);
#endif
		if (!myglfunc[i])
			return(0);
	}
	return(1);
}
