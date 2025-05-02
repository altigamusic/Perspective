//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2015                                   //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include "intro.h"
#include "ext.h"
#include "fp.h"
#include "glext.h"
#include "mzk.h"
#include "system.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <windows.h>

#ifndef _DEBUG
#include "shader.inl"
#endif

#ifdef REL_DEBUG
#include "shader.inl"
#endif

#if _DEBUG
#include "imgui/imgui.h"
#endif

#define STR1(x) #x
#define STR(x) STR1(x)

enum Scenes
{
    SCENE_SPHERE_ROOM = 0,
    SCENE_PERSPECTIVE_TEXT = 1,
    SCENE_PYRAMID = 2,
    SCENE_ROAD = 3,
    SCENE_JUNGLE = 4,
    SCENE_STAR = 5,
    SCENE_REVISION = 6
};

enum SkyColors
{
    SKY_SYNTHWAVE = 0,
    SKY_NIGHT = 1,
    SKY_BLUE_TO_WHITE = 2,
    SKY_SUNSET = 3
};

//---------------------------------------------------------------------

constexpr float fieldNearZ = 0.005f;
constexpr float fieldFarZ = 1000.0f;
constexpr float lightBound = 20;

constexpr float ALMOST_PI = 3.14f; // Slightly smaller than pi because that's good for things

//----------------------------------------

GLuint fragmentShaderProgram;
GLuint renderTexture;  // The texture that's rendered to
GLuint displayTexture; // The texture that's input to the shader
GLuint renderFramebuffer;
GLuint colorTextureUniform;
GLuint depthTextureUniform;
GLuint objectColorUniform;
GLuint timeUniform;
GLuint cameraPositionUniform;
GLuint cameraTargetUniform;
GLuint projectionPositionUniform;
GLuint projectionTargetUniform;
GLuint s0Uniform, s1Uniform, s2Uniform, s3Uniform, s4Uniform, s5Uniform;

GLuint sceneUniform;

// gl_TextureMatrix[0] = projection & view matrix
// gl_ModelViewMatrix = model matrix (not view!)
// static const char* vertexShaderSource =
//    "varying vec4 pos;"
//    "varying vec3 norm;"
//    "void main()"
//    "{"
//    "pos = gl_ModelViewMatrix * gl_Vertex;"
//    "norm = (gl_ModelViewMatrix * vec4(gl_Normal, 0)).xyz;"
//    "gl_Position = gl_TextureMatrix[0] * pos;"
//    "}";

#ifdef _DEBUG
void getDebugErrors(GLuint program)
{
    int length;
    char infoLog[512];

    glGetProgramInfoLog(program, 512, &length, infoLog);

    if (length > 0)
    {
        MessageBox(0, infoLog, "Info", MB_OK | MB_ICONEXCLAMATION);
    }

    // glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // if (!success)
    //{
    //     glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    //     MessageBox(0, infoLog, "Vertex Shader Error", MB_OK | MB_ICONEXCLAMATION);
    // };
    //
    // glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    // if (!success)
    //{
    //     glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    //     MessageBox(0, infoLog, "Fragment Shader Error", MB_OK | MB_ICONEXCLAMATION);
    // };
}
#endif

#ifndef VAR__t
#define VAR__t "_t"
#define VAR__tex0 "_tex0"
#define VAR__cp "_cp"
#define VAR__ct "_ct"
#define VAR__pp "_pp"
#define VAR__pt "_pt"
#define VAR_scene "scene"
#define VAR__s0 "_s0"
#define VAR__s1 "_s1"
#define VAR__s2 "_s2"
#define VAR__s3 "_s3"
#define VAR__s4 "_s4"
#define VAR__s5 "_s5"

#endif

constexpr float TEX_SCALE = 4;
constexpr float TEXW = XRES * TEX_SCALE;
constexpr float TEXH = YRES * TEX_SCALE;

void initFragmentShader(const char* fragmentShaderSource)
{
#if _DEBUG
    // In debug, the programs could get reloaded
    if (fragmentShaderProgram != 0)
    {
        glDeleteProgram(fragmentShaderProgram);
    }
#endif

    fragmentShaderProgram = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &fragmentShaderSource);

#if _DEBUG
    getDebugErrors(fragmentShaderProgram);
#endif

    timeUniform = glGetUniformLocation(fragmentShaderProgram, VAR__t);
    colorTextureUniform = glGetUniformLocation(fragmentShaderProgram, VAR__tex0);
    cameraPositionUniform = glGetUniformLocation(fragmentShaderProgram, VAR__cp);
    cameraTargetUniform = glGetUniformLocation(fragmentShaderProgram, VAR__ct);
    projectionPositionUniform = glGetUniformLocation(fragmentShaderProgram, VAR__pp);
    projectionTargetUniform = glGetUniformLocation(fragmentShaderProgram, VAR__pt);
    sceneUniform = glGetUniformLocation(fragmentShaderProgram, VAR_scene);
    s0Uniform = glGetUniformLocation(fragmentShaderProgram, VAR__s0);
    s1Uniform = glGetUniformLocation(fragmentShaderProgram, VAR__s1);
    s2Uniform = glGetUniformLocation(fragmentShaderProgram, VAR__s2);
    s3Uniform = glGetUniformLocation(fragmentShaderProgram, VAR__s3);
    s4Uniform = glGetUniformLocation(fragmentShaderProgram, VAR__s4);
    s5Uniform = glGetUniformLocation(fragmentShaderProgram, VAR__s5);
    glUseProgram(fragmentShaderProgram);
}

int intro_init(void)
{
    // Init the display texture
    glGenTextures(1, &displayTexture);
    glBindTexture(GL_TEXTURE_2D, displayTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXW, TEXH, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, displayTexture, 0);

#if _DEBUG
    // Check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        MessageBox(0, "Framebuffer incomplete after display texture.", "Framebuffer Error", MB_OK | MB_ICONEXCLAMATION);
        return 0;
    }
#endif

    // Init render texture
    // Init the framebuffer for that
    renderFramebuffer = 0;
    glGenFramebuffers(1, &renderFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, renderFramebuffer);

    glGenTextures(1, &renderTexture);
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXW, TEXH, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

#if _DEBUG
    // Check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        MessageBox(0, "Framebuffer incomplete after render texture.", "Framebuffer Error", MB_OK | MB_ICONEXCLAMATION);
        return 0;
    }
#endif

    // glGenTextures(1, &depthTexture);
    // glBindTexture(GL_TEXTURE_2D, depthTexture);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    // #if _DEBUG
    //     // Check that our framebuffer is ok
    //     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //     {
    //         MessageBox(0, "Framebuffer incomplete after depth texture.", "Framebuffer Error", MB_OK | MB_ICONEXCLAMATION);
    //         return 0;
    //     }
    // #endif

    return 1;
}

void render(const float ftime)
{
    glUniform1f(timeUniform, ftime);
    glRects(-1, -1, 1, 1);
}

void renderToScreen(const float ftime, IntroParams params)
{
    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, XRES, YRES);
    glUniform1f(s3Uniform, 1);

    glUniform3f(cameraPositionUniform, params.cameraPosition.x, params.cameraPosition.y, params.cameraPosition.z);
    glUniform3f(cameraTargetUniform, params.cameraTarget.x, params.cameraTarget.y, params.cameraTarget.z);
    glUniform3f(projectionPositionUniform, params.projectionPosition.x, params.projectionPosition.y, params.projectionPosition.z);
    glUniform3f(projectionTargetUniform, params.projectionTarget.x, params.projectionTarget.y, params.projectionTarget.z);
    glUniform1i(sceneUniform, params.scene);
    glUniform1f(s0Uniform, params.s0);
    glUniform1f(s1Uniform, params.s1);

    glUniform1i(colorTextureUniform, 0);

    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, depthTexture); // Load depth texture into texture 1

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, displayTexture);

    // glClearColor(0.392f, .584f, .929f, 1.0f);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render(ftime);
}

void renderToTexture(const float ftime, IntroParams params)
{
    // Render to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, renderFramebuffer);
    glViewport(0, 0, TEXW, TEXH);
    glUniform1f(s3Uniform, TEX_SCALE);

    glUniform3f(cameraPositionUniform, params.cameraPosition.x, params.cameraPosition.y, params.cameraPosition.z);
    glUniform3f(cameraTargetUniform, params.cameraTarget.x, params.cameraTarget.y, params.cameraTarget.z);
    glUniform3f(projectionPositionUniform, params.projectionPosition.x, params.projectionPosition.y, params.projectionPosition.z);
    glUniform3f(projectionTargetUniform, params.projectionTarget.x, params.projectionTarget.y, params.projectionTarget.z);
    glUniform1i(sceneUniform, params.scene);

    glUniform1f(s0Uniform, params.s0);

    glUniform1i(colorTextureUniform, 0);

    // Set the display texture as texture 0
    // The render texture is bound to the framebuffer (I think), so it should be rendered to
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, displayTexture);

    // glClearColor(0, 0, 0, 0);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render(ftime);

    // Now copy the texture to the display texture
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, TEXW, TEXH, 0);
}

float easeIn(float x) { return x * x * (2 - x); }
float easeOut(float x) { return 1 - easeIn(1 - x); }
float smoothstep(float x) { return x * x * (3 - 2 * x); }

float gain(float x, float k)
{
    float a = 0.5 * powX(2.0 * ((x < 0.5) ? x : 1.0 - x), k);
    return (x < 0.5) ? a : 1.0 - a;
}

IntroParams scene1(float t)
{
    vec3 pos = {0, 0, easeOut(t) * 12 - 2.f};
    vec3 target = pos - vec3{0, 0, 1};

    return {
        SKY_NIGHT, 0, SCENE_PERSPECTIVE_TEXT, pos, target, {0, 0, 10},
             {0, 0, 0 }
    };
}

IntroParams scene2(float t)
{
    vec3 pos = {3.86f, 4.93f, -15.89f};

    // Rotate XZ (lazy-style)
    pos = pos.xzy().rotateXY(t).xzy();

    vec3 target = {0, 0, 0};

    return {
        0, 0, SCENE_SPHERE_ROOM, pos, target, {0, 0, 50 },
             {0, 0, 100}  // No need for projecting on this scene, so we just push it far away
    };
}

IntroParams scene3(float t)
{
    vec3 pos = {0, 10, 0};
    vec3 target = {0, 0, 0};

    pos = pos.rotateXY(easeOut(t) * 1.5); // Almost-almost-pi / 2

    return {
        SKY_SUNSET, 0, SCENE_PYRAMID, pos, target, {0,    10, 0},
             {0.01, 0,  0}
    };
}

IntroParams scene4(float t)
{
    t = min(t, 1); // Rounding problems
    vec3 pos = {0, 0, smoothstep(t) * 100};
    vec3 direction = {0, 0, 1};
    direction = direction.rotateXZ(gain(t, 7) * ALMOST_PI);
    vec3 target = pos + direction;

    return {
        SKY_NIGHT, 0, SCENE_ROAD, pos, target, {0, 0, 0},
             {0, 0, 1}
    };
}

IntroParams scene5(float t)
{
    t = min(t, 1); // Rounding problems
    vec3 pos = {0, 0, easeOut(t) * 100};
    vec3 direction = {0, 0, 1};
    direction = direction.rotateXZ(gain(t, 7) * ALMOST_PI);
    vec3 target = pos + direction;

    return {
        SKY_SYNTHWAVE, 0, SCENE_JUNGLE, pos, target, {0, 0, 0},
             {0, 0, 1}
    };
}

int prevEighth = 0;
IntroParams prevResult;

IntroParams scene6(float t)
{
    int eighth = f2i(t * 31); // This is probably gonna require fixing later >:(
    float fract = t * 31 - eighth;
    fract = (fract - .5) * .4;

    // 1, -2, -1, 2, repeat
    // int id = (2 - (eighth & 1)) * (((eighth + 1) & 2) - 1);
    int id = eighth >= 16 ? -1 : 1;

    vec3 pos = {0, 0, 15};
    bool right = eighth < 8 || (eighth >= 16 && eighth < 24);
    vec3 init = right ? pos.rotateXZ(-.2 * id) : pos.rotateYZ(-.2 * id);
    pos = right ? pos.rotateXZ(fract * id) : pos.rotateYZ(fract * id);
    vec3 target = {0, 0, 0};
    IntroParams result = {eighth % 3 + 1, 0, SCENE_SPHERE_ROOM, pos, target, init, target - init};

    if (eighth > prevEighth)
    {
        // Render the current position and reset
        renderToTexture(0, prevResult);
        prevEighth = eighth;
    }

    prevResult = result;
    return result;
}

IntroParams scene7(float t)
{
    int eighth = f2i(t * 31); // This is probably gonna require fixing later >:(
    float fract = t * 31 - eighth;
    fract = (easeIn(fract) - .5) * .4;

    // int id = eighth % 4 - 2;
    // id = id >= 0 ? id + 1 : id;
    int id = eighth;

    vec3 pos = {0, 0, 15};
    vec3 init = pos.rotateXZ(-.2 + id);
    pos = pos.rotateXZ(fract + id);
    vec3 target = {0, 0, 0};
    IntroParams result = {eighth % 2, (eighth / 2) % 3, SCENE_STAR, pos, target, init, init * -1};

    if (eighth > prevEighth)
    {
        // Render the current position and reset
        renderToTexture(t, prevResult);
        prevEighth = eighth;
    }

    prevResult = result;
    return result;
}

IntroParams scene8(float t)
{
    vec3 pos = {0, 0, easeOut(1 - t) * 10.5f - .5f};
    vec3 target = pos - vec3{0, 0, 1};

    return {
        SKY_SUNSET, 0, SCENE_PERSPECTIVE_TEXT, pos, target, {0, 0, 10},
             {0, 0, 0 }
    };
}

IntroParams scene9(float t)
{
    float factor = easeOut(easeIn(min(t / 16,1))) * ALMOST_PI;
    vec3 pos = vec3{0, 4, 0}.rotateYZ(-factor);
    vec3 target = vec3{0, 0, 0.01f};

    float fade = (20 - t) / 2.;
    fade = min(1, max(0, fade));
    fade = smoothstep(fade);

    return {
        2, fade, SCENE_REVISION, pos, target, {0, 4, 0    },
             {0, 0, 0.01f}
    };
}

int currentPart = -1;

void intro_do(long itime, IntroParams params, bool overrideControl)
{
    float ftime = 0.001f * (float)itime;

    if (params.scene != currentPart)
    {
        // Trigger init
        switch (params.scene + 1)
        {
        case 1:
            // Render the start of scene 2 into the texture
            renderToTexture(0, scene2(0));
            break;
        case 2:
            // Do nothing
            break;
        case 3:
            // Render the end of scene 2 into the texture
            renderToTexture(1, scene2(1));
            break;
        case 4:
            // Render the end of scene 3 into the texture
            renderToTexture(1, scene3(1));
            break;
        case 5:
            renderToTexture(1, scene4(1));
            break;
        case 6:
            renderToTexture(1, scene5(1));
            prevEighth = 0;
            break;
        case 7:
            renderToTexture(1, scene6(1));
            prevEighth = 0;
            break;
        case 8:
            renderToTexture(1, scene7(1));
            break;
        case 9:
            renderToTexture(1, scene8(1));
            break;
        }
    }

    currentPart = params.scene;

    ftime /= 15.5f; // The amount of time in each scene

    IntroParams sceneParams;

    switch (params.scene + 1)
    {
    case 1:
        sceneParams = scene1(ftime);
        break;
    case 2:
        sceneParams = scene2(ftime);
        break;
    case 3:
        sceneParams = scene3(ftime);
        break;
    case 4:
        sceneParams = scene4(ftime);
        break;
    case 5:
        sceneParams = scene5(ftime);
        break;
    case 6:
        sceneParams = scene6(ftime);
        break;
    case 7:
    default:
        sceneParams = scene7(min(1,ftime));
        break;
    case 8:
        sceneParams = scene8(ftime);
        break;
    case 9:
        sceneParams = scene9(ftime * 15.5f);
        break;
    }

    // sceneParams.s0 = params.s0;

    if (overrideControl)
    {
        sceneParams.cameraPosition = params.cameraPosition;
        sceneParams.cameraTarget = params.cameraTarget;
    }

    renderToScreen(ftime, sceneParams);

    // if (params.rerender)
    //{
    //     renderToTexture(ftime, params);
    // }

    // renderToScreen(ftime, params);

#if _DEBUG
    glBindFramebuffer(GL_READ_FRAMEBUFFER, renderFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, 1024, 1024, 0, 0, 100, 100, GL_COLOR_BUFFER_BIT, GL_NEAREST);
#endif

    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}
