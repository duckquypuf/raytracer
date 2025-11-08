#pragma once

#include <glm/glm.hpp>

struct Body
{
    glm::vec3 position;
    float radius;
    glm::vec4 colour;
    float reflectiveness;
    float roughness;
    bool isLight;
};
