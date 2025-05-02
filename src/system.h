//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2015                                   //
//--------------------------------------------------------------------------//

#ifndef _SYSTEM_H_
#define _SYSTEM_H_


#ifdef WINDOWS
static inline int f2i2(float x) // use this to convert float to int, or use /QIfist as additional compiler parameters
{
    int tmp;
    _asm fld dword ptr[x]
    _asm fistp dword ptr[tmp];
    return tmp;
}

static inline int f2i(float x) {
    return f2i2(x - 0.5f);
}
/*
static inline int mysinf(float x)
{
    int tmp;
    _asm fld dword ptr[x]
    _asm fsin
    _asm fstp dword ptr[tmp];
    return tmp;
}*/

static inline float powX(float base, float exponent)
{
    if (base == 0 && exponent != 0) return 0;
    float result;

    _asm {
        fld exponent
        fld base
        fyl2x
        fld1
        fld st(1)
        fprem
        f2xm1
        fadd
        fscale
        fxch st(1)
        fstp st
        fstp result
    }
    return result;
}

#endif

#endif