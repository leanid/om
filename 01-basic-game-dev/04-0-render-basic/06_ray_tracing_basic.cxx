#include <vector>

#include <glm/glm.hpp> // sudo dnf install glm-devel

#include "00_canvas_basic.hxx"

const glm::vec3 O{ 0.f, 0.f, 0.f };

const float d{ 1.f };

const float Cw{ 640.f }; /// canvas width, num of pixels

const float Ch{ 640.f }; /// canvas height, num of pixels

const float Vw{ d }; /// viewport width in 3D space
const float Vh{ d }; /// viewport height in 3D space

const float inf{ std::numeric_limits<float>::infinity() }; /// c++ infinity

/// position in 3D space of pixel from canvas
glm::vec3 canvas_to_viewport(int pixel_x, int pixel_y)
{
    return glm::vec3{ pixel_x * Vw / Cw, pixel_y * Vh / Ch, d };
}

using color_t = glm::vec3;

constexpr color_t red{ 1.f, 0.f, 0.f };
constexpr color_t green{ 0.f, 1.f, 0.f };
constexpr color_t blue{ 0.f, 0.f, 1.f };
constexpr color_t yellow{ 1.f, 1.f, 0.f };
constexpr color_t background{ 1.f, 1.f, 1.f };

struct sphere_t
{
    glm::vec3 center_position;
    color_t   color;
    float     radius;
};

color_t ray_trace(const glm::vec3&             origin,
                  const glm::vec3&             direction,
                  float                        start,
                  float                        inf,
                  const std::vector<sphere_t>& objects);

void canvas_put_pixel(int x, int y, color_t col, canvas& image)
{
    const size_t image_x = (Cw / 2) + x;
    const size_t image_y = (Ch / 2) - y;

    if (image_x >= Cw || image_y >= Ch)
    {
        return;
    }

    const color c{ .r=static_cast<uint8_t>(col.r * 255),
                   .g=static_cast<uint8_t>(col.g * 255),
                   .b=static_cast<uint8_t>(col.b * 255) };

    image.set_pixel(image_x, image_y, c);
}

int main(int argc, char** argv)
{
    canvas image(Cw, Ch);

    std::vector<sphere_t> scene;

    scene.push_back(sphere_t{ .center_position=glm::vec3{ 0.f, -1.f, 3.f }, .color=red, .radius=1.f });
    scene.push_back(sphere_t{ .center_position=glm::vec3{ 2.f, 0.f, 4.f }, .color=blue, .radius=1.f });
    scene.push_back(sphere_t{ .center_position=glm::vec3{ -2.f, 0.f, 4.f }, .color=green, .radius=1.f });
    scene.push_back(sphere_t{ .center_position=glm::vec3{ 0.f, -5001.f, 0.f }, .color=yellow, .radius=5000.f });

    for (int x = -Cw / 2; x < Cw / 2; ++x)
    {
        for (int y = -Ch / 2; y < Ch / 2; ++y)
        {
            const glm::vec3 Direction{ canvas_to_viewport(x, y) };
            const auto      color{ ray_trace(O, Direction, 1.f, inf, scene) };
            canvas_put_pixel(x, y, color, image);
        }
    }

    image.save_image("06_ray_tracing_basic.ppm");
    return 0;
}

struct intersection
{
    float t_0;
    float t_1;
};

intersection ray_intersect_sphere(const glm::vec3& ray_start,
                                  const glm::vec3& ray_direction,
                                  const sphere_t&  sphere)
{
    const glm::vec3 T{ ray_start - sphere.center_position };
    float           a = glm::dot(ray_direction, ray_direction);
    float           b = 2 * glm::dot(T, ray_direction);
    float           c = glm::dot(T, T) - sphere.radius * sphere.radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        return intersection{ .t_0=inf, .t_1=inf };
    }

    float t1 = (-b + std::sqrt(discriminant)) / (2 * a);
    float t2 = (-b - std::sqrt(discriminant)) / (2 * a);

    return intersection{ .t_0=t1, .t_1=t2 };
}

color_t ray_trace(const glm::vec3&             origin,
                  const glm::vec3&             direction,
                  float                        start_t,
                  float                        end_t,
                  const std::vector<sphere_t>& objects)
{
    const sphere_t* closest   = nullptr;
    float           closest_t = inf;

    for (const sphere_t& sphere : objects)
    {
        const auto [t1, t2] = ray_intersect_sphere(origin, direction, sphere);
        if (t1 >= start_t && t1 <= end_t && t1 < closest_t)
        {
            closest   = &sphere;
            closest_t = t1;
        }
        if (t2 >= start_t && t2 <= end_t && t2 < closest_t)
        {
            closest   = &sphere;
            closest_t = t2;
        }
    }

    if (closest == nullptr)
    {
        return background;
    }

    return closest->color;
}
