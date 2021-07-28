#include <glm/glm.hpp>

const glm::vec3 O{ 0.f, 0.f, 0.f };

const float d{ 1.f };

const float Cw{ 640.f }; /// canvas width, num of pixels

const float Ch{ 480.f }; /// canvas height, num of pixels

const float Vw{ d }; /// viewport width in 3D space
const float Vh{ d }; /// viewport height in 3D space

const float inf{ std::numeric_limits<float>::infinity() }; /// c++ infinity

/// position in 3D space of pixel from canvas
glm::vec3 canvas_to_viewport(int pixel_x, int pixel_y)
{
    return glm::vec3{ pixel_x * Vw / Cw, pixel_y * Vh / Ch, d };
}

using color = glm::vec3;

color ray_trace(const glm::vec3& origin,
                const glm::vec3& direction,
                float            start,
                float            inf);

void canvas_put_pixel(int x, int y, color);

int main(int argc, char** argv)
{
    for (int x = -Cw / 2; x < Cw / 2; ++x)
    {
        for (int y = -Ch / 2; y < Ch / 2; ++y)
        {
            glm::vec3 Direction{ canvas_to_viewport(x, y) };
            auto      color{ ray_trace(O, Direction, 1.f, inf) };
            canvas_put_pixel(x, y, color);
        }
    }
    return 0;
}
