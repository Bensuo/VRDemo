#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>

struct Material
{
    glm::vec3 Diffuse;
    glm::vec3 Ambient;
    glm::vec3 Specular;
    float Shininess;
};
#endif // MATERIAL_H