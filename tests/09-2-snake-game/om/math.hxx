#pragma once

#ifndef OM_DECLSPEC
#define OM_DECLSPEC
#endif

namespace om
{

struct OM_DECLSPEC vec2
{
    vec2();
    vec2(float x, float y);
    float x = 0;
    float y = 0;
    vec2& operator+=(const vec2& l);
};

vec2 OM_DECLSPEC operator+(const vec2& l, const vec2& r);

struct OM_DECLSPEC matrix
{
    matrix();
    static matrix identity();
    static matrix scale(float scale);
    static matrix scale(float sx, float sy);
    static matrix rotation(float thetha);
    static matrix move(const vec2& delta);
    vec2          row0;
    vec2          row1;
    vec2          row2;
};

vec2 OM_DECLSPEC operator*(const vec2& v, const matrix& m);
matrix OM_DECLSPEC operator*(const matrix& m1, const matrix& m2);

} // end namespace om
