#include "00_canvas_basic.hxx"

irender::~irender() {}

bool operator==(const color& l, const color& r)
{
    return l.r == r.r && l.g == r.g && l.b == r.b;
}

constexpr size_t color_size = sizeof(color);

static_assert(3 == color_size, "24 bit per pixel(r,g,b)");

position operator-(const position& left, const position& right)
{
    return { left.x - right.x, left.y - right.y };
}

bool operator==(const position& left, const position& right)
{
    return left.x == right.x && left.y == right.y;
}

position position::generate_random(int width, int height)
{
    return { rand() % width, rand() % height };
}
