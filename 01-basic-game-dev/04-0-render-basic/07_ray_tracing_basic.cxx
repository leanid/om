#include <variant>
#include <vector>

#include <glm/glm.hpp>

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
constexpr color_t background{ 1.f, 1.f, 1.f };
constexpr color_t yellow{ 1.f, 1.f, 0.f };

struct sphere_t
{
    glm::vec3 center_position;
    color_t   color;
    float     radius;
};

struct light_t
{
    enum class type : uint32_t
    {
        ambient     = 0,
        point       = 1,
        directional = 2
    };

    struct ambient
    {
        float intensity;
    };

    struct point
    {
        glm::vec3 position;
        float     intensity;
    };

    struct directional
    {
        glm::vec3 direction;
        float     intensity;
    };

    std::variant<ambient, point, directional> info;

    type get_type() const { return static_cast<type>(info.index()); }
};

color_t ray_trace(const glm::vec3&             origin,
                  const glm::vec3&             direction,
                  float                        start,
                  float                        inf,
                  const std::vector<sphere_t>& objects,
                  const std::vector<light_t>&  lights);

// return light intensity
float compute_lighting(const glm::vec3             P,
                       const glm::vec3&            N,
                       const std::vector<light_t>& lights);

void canvas_put_pixel(int x, int y, color_t col, canvas& image)
{
    const size_t image_x = (Cw / 2) + x;
    const size_t image_y = (Ch / 2) - y;

    if (image_x >= Cw || image_y >= Ch)
    {
        return;
    }

    const color c{ static_cast<uint8_t>(col.r * 255),
                   static_cast<uint8_t>(col.g * 255),
                   static_cast<uint8_t>(col.b * 255) };

    image.set_pixel(image_x, image_y, c);
}

int main(int argc, char** argv)
{
    canvas image(Cw, Ch);

    std::vector<sphere_t> scene;

    scene.push_back(sphere_t{ glm::vec3{ 0.f, -1.f, 3.f }, red, 1.f });
    scene.push_back(sphere_t{ glm::vec3{ 2.f, 0.f, 4.f }, blue, 1.f });
    scene.push_back(sphere_t{ glm::vec3{ -2.f, 0.f, 4.f }, green, 1.f });
    scene.push_back(sphere_t{ glm::vec3{ 0.f, -5001.f, 0.f }, yellow, 5000.f });

    std::vector<light_t> lights;

    lights.push_back(light_t{ light_t::ambient{ 0.2f } });
    lights.push_back(
        light_t{ light_t::point{ glm::vec3{ 2.f, 1.f, 0.f }, 0.6f } });
    lights.push_back(
        light_t{ light_t::directional{ glm::vec3{ 1.f, 4.f, 4.f }, 0.2f } });

    for (int x = -Cw / 2; x < Cw / 2; ++x)
    {
        for (int y = -Ch / 2; y < Ch / 2; ++y)
        {
            glm::vec3 Direction{ canvas_to_viewport(x, y) };
            auto      color{ ray_trace(O, Direction, 1.f, inf, scene, lights) };
            canvas_put_pixel(x, y, color, image);
        }
    }

    image.save_image("07_ray_tracing_basic.ppm");
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
        return intersection{ inf, inf };
    }

    float t1 = (-b + std::sqrt(discriminant)) / (2 * a);
    float t2 = (-b - std::sqrt(discriminant)) / (2 * a);

    return intersection{ t1, t2 };
}

color_t ray_trace(const glm::vec3&             origin,
                  const glm::vec3&             direction,
                  float                        start,
                  float                        inf,
                  const std::vector<sphere_t>& objects,
                  const std::vector<light_t>&  lights)
{
    const sphere_t* closest   = nullptr;
    float           closest_t = inf;

    for (const sphere_t& sphere : objects)
    {
        const auto [t1, t2] = ray_intersect_sphere(origin, direction, sphere);
        if (t1 > 1.f && t1 < inf && t1 < closest_t)
        {
            closest   = &sphere;
            closest_t = t1;
        }
        if (t2 > 1.f && t2 < t1 && t2 < closest_t)
        {
            closest   = &sphere;
            closest_t = t2;
        }
    }

    if (closest == nullptr)
    {
        return background;
    }

    const glm::vec3 P = O + closest_t * glm::normalize(direction);
    const glm::vec3 N = glm::normalize(P - closest->center_position);

    const float intensity = compute_lighting(P, N, lights);

    return closest->color * intensity;
}

float compute_lighting(const glm::vec3             P,
                       const glm::vec3&            N,
                       const std::vector<light_t>& lights)
{
    float intensity = 0.f;
    for (const light_t& light : lights)
    {
        const light_t::type type = light.get_type();
        if (type == light_t::type::ambient)
        {
            intensity += std::get<light_t::ambient>(light.info).intensity;
        }
        else
        {
            glm::vec3 L;
            float     light_intensity;
            if (type == light_t::type::point)
            {
                const light_t::point& p = std::get<light_t::point>(light.info);
                L                       = p.position - P;
                light_intensity         = p.intensity;
            }
            else
            {
                const light_t::directional& p =
                    std::get<light_t::directional>(light.info);
                L               = p.direction;
                light_intensity = p.intensity;
            }
            const float n_dot_l = glm::dot(N, L);
            if (n_dot_l > 0.f) // angle < 90 degrees or skip
            {
                intensity += light_intensity * n_dot_l /
                             (glm::length(N) * glm::length(L));
            }
        }
    }
    return intensity;
}
