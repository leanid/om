#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "gles30_texture.hxx"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};
