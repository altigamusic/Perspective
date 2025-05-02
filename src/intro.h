//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2015                                   //
//--------------------------------------------------------------------------//

#ifndef _INTRO_H_
#define _INTRO_H_

#include <math.h>

#define XRES 1920
#define YRES 1080

int intro_init(void);
void renderCamera(const float ftime);
void renderLight(const float ftime);
void render(const float ftime);
void intro_end(void);

typedef struct _vec3
{
    float x;
    float y;
    float z;

    inline _vec3 operator+(_vec3 a) { return {a.x + x, a.y + y, a.z + z}; }

    inline _vec3 operator+(float s) { return {x + s, y + s, z + s}; }

    inline _vec3 operator-(_vec3 a) { return {x - a.x, y - a.y, z - a.z}; }

    inline _vec3 operator-(float s) { return {x - s, y - s, z - s}; }

    inline _vec3 operator*(_vec3 a) { return {a.x * x, a.y * y, a.z * z}; }

    inline _vec3 operator*(float s) { return {x * s, y * s, z * s}; }

    inline void operator*=(_vec3 a)
    {
        x *= a.x;
        y *= a.y;
        z *= a.z;
    }

    inline void operator*=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
    }

    inline struct _vec3 cross(struct _vec3 other)
    {
        return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
    }

    inline _vec3 normalize()
    {
        // Normalize
        return *this * (1.0f / sqrt(x * x + y * y + z * z));
    }

    inline _vec3 xzy() { return {x, z, y}; }
    inline _vec3 yxz() { return {y, x, z}; }
    inline _vec3 yzx() { return {y, z, x}; }
    inline _vec3 zxy() { return {z, x, y}; }
    inline _vec3 zyx() { return {z, y, x}; }

    inline _vec3 rotateXY(float angle)
    {
        float s = sinf(angle), c = cosf(angle);
        return {x * c - y * s, x * s + y * c, z};
    }

    inline _vec3 rotateXZ(float angle)
    {
        float s = sinf(angle), c = cosf(angle);
        return {x * c - z * s, y, x * s + z * c};
    }

    inline _vec3 rotateYZ(float angle)
    {
        float s = sinf(angle), c = cosf(angle);
        return {x, y * c - z * s, y * s + z * c};
    }
} vec3;

typedef struct _vec4
{
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct _IntroParams
{
    float s0;
    float s1;
    int scene;
    vec3 cameraPosition;
    vec3 cameraTarget;
    vec3 projectionPosition;
    vec3 projectionTarget;
} IntroParams;

void intro_do(long time, IntroParams params, bool overrideControl);

void initFragmentShader(const char* fragmentShaderSource);

#endif
