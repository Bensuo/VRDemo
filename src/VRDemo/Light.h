#ifndef LIGHT_H
#define LIGHT_H

namespace Engine
{
    namespace Rendering
    {
        // inheritance based light heirarcy violated liskov substitution, so I have opted for discrete structs for each light caster

        struct DirectionalLight
        {
            glm::vec3 Ambient;
            glm::vec3 Diffuse;
            glm::vec3 Specular;

            glm::vec3 Direction;
        };

        struct PointLight
        {
            glm::vec3 Ambient;
            glm::vec3 Diffuse;
            glm::vec3 Specular;

            glm::vec3 Position;

            float Constant;
            float Linear;
            float Quadratic;
        };

        struct SpotLight
        {
            glm::vec3 Ambient;
            glm::vec3 Diffuse;
            glm::vec3 Specular;

            glm::vec3 Position;
            glm::vec3 Direction;

            float CutOff;
            float OuterCutOff;

            float Constant;
            float Linear;
            float Quadratic;
        };
    }
}
#endif // LIGHT_H