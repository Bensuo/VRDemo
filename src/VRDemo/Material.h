#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Code is from Stuart Adams' individual coursework for Advanced Graphics
*/

struct Material
{
    glm::vec3 Diffuse;
    glm::vec3 Ambient;
    glm::vec3 Specular;
    float Shininess;
};
#endif // MATERIAL_H
