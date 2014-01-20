//
// Keijiro Takahashi (C) 2013, 2014
// https://github.com/keijiro/kvant
//
// The implementation of the noise functions are
// based on the Java implementation by Ken Perlin
// http://mrl.nyu.edu/~perlin/noise/
//

#include <cmath>

namespace
{
    int perm[] = {
        151,160,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
        151
    };
    
    int f2i(float x)
    {
        return static_cast<int>(std::floorf(x));
    }
    
    float fade(float t)
    {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }
    
    float lerp(float t, float a, float b)
    {
        return a + t * (b - a);
    }
    
    float grad(int i, float x)
    {
        return (perm[i] & 1) ? x : -x;
    }
    
    float grad(int i, float x, float y)
    {
        auto h = perm [i];
        return ((h & 1) ? x : -x) + ((h & 2) ? y : -y);
    }
    
    float grad(int i, float x, float y, float z)
    {
        auto h = perm [i] & 15;
        auto u = h < 8 ? x : y;
        auto v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
        return ((h & 1) ? u : -u) + ((h & 2) ? v : -v);
    }
    
    float noise(float x)
    {
        auto X = f2i(x);
        x -= X;
        X &= 0xff;
        return lerp(fade(x), grad(X, x), grad (X+1, x-1));
    }
    
    float noise(float x, float y)
    {
        auto X = f2i(x);
        auto Y = f2i(y);
        x -= X;
        y -= Y;
        X &= 0xff;
        Y &= 0xff;
        auto u = fade(x);
        auto v = fade(y);
        auto A = (perm[X  ] + Y) & 0xff;
        auto B = (perm[X+1] + Y) & 0xff;
        return lerp(v, lerp(u, grad(A  , x, y  ), grad(B  , x-1, y  )),
                    lerp(u, grad(A+1, x, y-1), grad(B+1, x-1, y-1)));
    }
    
    float noise(float x, float y, float z)
    {
        auto X = f2i(x);
        auto Y = f2i(y);
        auto Z = f2i(z);
        x -= X;
        y -= Y;
        z -= Z;
        X &= 0xff;
        Y &= 0xff;
        Z &= 0xff;
        auto u = fade(x);
        auto v = fade(y);
        auto w = fade(z);
        auto A  = (perm[X  ] + Y) & 0xff;
        auto B  = (perm[X+1] + Y) & 0xff;
        auto AA = (perm[A  ] + Z) & 0xff;
        auto BA = (perm[B  ] + Z) & 0xff;
        auto AB = (perm[A+1] + Z) & 0xff;
        auto BB = (perm[B+1] + Z) & 0xff;
        return lerp(w, lerp(v, lerp(u, grad(AA  , x, y  , z  ), grad(BA  , x-1, y  , z  )),
                            lerp(u, grad(AB  , x, y-1, z  ), grad(BB  , x-1, y-1, z  ))),
                    lerp(v, lerp(u, grad(AA+1, x, y  , z-1), grad(BA+1, x-1, y  , z-1)),
                         lerp(u, grad(AB+1, x, y-1, z-1), grad(BB+1, x-1, y-1, z-1))));
    }
}

extern "C" float KvantNoise1D(float x)
{
    return noise(x);
}

extern "C" float KvantNoise2D(float x, float y)
{
    return noise(x, y);
}

extern "C" float KvantNoise3D(float x, float y, float z)
{
    return noise(x, y, z);
}

extern "C" float KvantFBM1D(float x, int octave)
{
    auto f = 0.0f;
    auto w = 0.5f;
    for (int i = 0; i < octave; i++) {
        f += w * noise(x);
        x *= 2.0f;
        w *= 0.5f;
    }
    return f;
}

extern "C" float KvantFBM2D(float x, float y, int octave)
{
    auto f = 0.0f;
    auto w = 0.5f;
    for (int i = 0; i < octave; i++) {
        f += w * noise(x, y);
        x *= 2.0f;
        y *= 2.0f;
        w *= 0.5f;
    }
    return f;
}

extern "C" float KvantFBM3D(float x, float y, float z, int octave)
{
    auto f = 0.0f;
    auto w = 0.5f;
    for (int i = 0; i < octave; i++) {
        f += w * noise(x, y, z);
        x *= 2.0f;
        y *= 2.0f;
        z *= 2.0f;
        w *= 0.5f;
    }
    return f;
}

extern "C" float KvantFractal4Coeffs(float x, float y, float z, float w0, float w1, float w2, float w3)
{
    auto f = w0 * noise(x, y, z);
    x *= 2.0f; y *= 2.0f; z *= 2.0f;
    f += w1 * noise(x, y, z);
    x *= 2.0f; y *= 2.0f; z *= 2.0f;
    f += w2 * noise(x, y, z);
    x *= 2.0f; y *= 2.0f; z *= 2.0f;
    return f + w3 * noise(x, y, z);
}
