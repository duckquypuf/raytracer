#pragma once

#include "body.h"
#include "maths.h"

#include <random>

static std::random_device rd;
static std::mt19937 generator(rd()); // Seeded once at program start

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
};

float intersectSphere(const Ray &ray, const Body &sphere)
{
    // t = (-b +- √b^2 - 4ac) / 2a
    glm::vec3 oc = ray.origin - sphere.position;

    // A term is 1.0 since ray.direction is normalized. Using glm::dot() for completeness.
    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - sphere.radius * sphere.radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
        return -1.0f;
    else
    {
        // t0 is the nearest intersection point
        float t0 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
        if (t0 > 0.001f)
            return t0;

        // t1 is the farther intersection point (used if camera is inside the sphere)
        float t1 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
        if (t1 > 0.001f)
            return t1;
    }
    return -1.0f;
}

glm::vec3 getRandomDirectionOnUnitSphere()
{
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    glm::vec3 p;

    do p = glm::vec3(dist(generator), dist(generator), dist(generator)); while (glm::dot(p, p) > 1.0f);

    return glm::normalize(p);
}

glm::vec3 randomDirectionInHemisphere(const glm::vec3 &normal)
{
    glm::vec3 randomDir = getRandomDirectionOnUnitSphere();

    if (glm::dot(randomDir, normal) < 0.0f) randomDir = -randomDir;

    return randomDir;
}

glm::vec3 traceRay(const Ray& ray, Body* bodies, int bodyCount, int depth)
{
    return glm::vec3(0.0f);
}
