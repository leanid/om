#include "graphics_basic.hxx"

std::array<color, buffer_size> image;

render::~render() {}

basic_render::basic_render(std::array<color, buffer_size>& buffer_,
                           size_t width, size_t height)
    : buffer(buffer_)
    , w(width)
    , h(height)
{
}

void basic_render::clear(color c)
{
    std::fill(begin(buffer), end(buffer), c);
}

void basic_render::set_pixel(position p, color c)
{
    color& col = buffer.at(p.y * w + p.x);
    col        = c;
}

void basic_render::draw_line(position start, position end, color c)
{
    // TODO
    set_pixel(start, c);
    set_pixel(end, c);
}

int main(int, char**)
{
    const color black = { 0, 0, 0 };
    const color white = { 255, 255, 255 };

    basic_render render(image, width, height);

    render.clear(black);

    render.draw_line(position{ 0, 0 }, position{ height - 1, height - 1 },
                     white);

    save_image("image.ppm", image);
    return 0;
}
