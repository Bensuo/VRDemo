#pragma once

#include "VRSystem.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
    struct MovementVectors
    {
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 World_up;
        glm::vec3 Movement_Dir;

        void Update(const glm::vec3& front)
        {
            Front = glm::normalize(front);
            Right = normalize(cross(Front, World_up));
            Up = normalize(cross(Right, Front));
        }
    };

    class VRPlayer
    {
        VRInputState input_state;
        std::shared_ptr<VRSystem> vr_system;
        Transform3D transform;
        MovementVectors movement_vectors;
    public:
        VRPlayer(std::shared_ptr<VRSystem> system)
            : vr_system(system), input_state()
        {
            movement_vectors.World_up = glm::vec3(0, 1, 0);
            movement_vectors.Update(vr_system->GetHMDTransform().GetFrontDirection() * transform.GetRotation());
        }

        Transform3D GetPlayerTransform()
        {
            return transform;
        }

        glm::mat4 GetEyeViewWorldSpace(int eye)
        {
            auto view = glm::mat4_cast(transform.GetRotation());
            view = glm::translate(view, -transform.GetPosition());
            return vr_system->GetViewFromEye(eye) * view;
        }

        void SetTransform(const Transform3D& transform)
        {
            this->transform = transform;
            movement_vectors.Update(vr_system->GetHMDTransform().GetFrontDirection() * transform.GetRotation());
        }

        Transform3D GetEyeTransformWorldspace(int eye)
        {
            Transform3D result;
            auto localEyeSpace = vr_system->GetEyeTransform(eye);

            result.SetPosition(localEyeSpace.GetPosition() + transform.GetPosition());
            result.SetRotation(localEyeSpace.GetRotation() * transform.GetRotation());

            return result;
        }

        glm::vec3 GetLeftHandDirWorldspace()
        {
            auto hand = input_state.GetLeft().Transform;
            return vr_system->GetInputState().GetLeft().Transform.GetFrontDirection() * transform.GetRotation();
        }

        glm::vec3 GetLeftHandPosWorldspace()
        {
            auto rot = glm::mat4_cast(transform.GetRotation());
            rot = glm::translate(rot, -transform.GetPosition());
            return glm::vec3(glm::vec4(input_state.GetLeft().Transform.GetPosition(), 1) * rot) + transform.GetPosition();
        }

        glm::vec3 GetRightHandDirWorldspace()
        {
            auto hand = input_state.GetRight().Transform;
            return vr_system->GetInputState().GetRight().Transform.GetFrontDirection() * transform.GetRotation();
        }

        glm::vec3 GetRightHandPosWorldspace()
        {
            auto rot = glm::mat4_cast(transform.GetRotation());
            rot = glm::translate(rot, -transform.GetPosition());
            return glm::vec3(glm::vec4(input_state.GetRight().Transform.GetPosition(), 1) * rot) + transform.GetPosition();
        }

        void Update(const GameTime delta_time)
        {
            input_state = vr_system->GetInputState();

            const auto velocity = movement_vectors.Movement_Dir * delta_time;

            auto new_pos = transform.GetPosition() + velocity;
            transform.SetPosition(new_pos);

            movement_vectors.Update(vr_system->GetHMDTransform().GetFrontDirection() * transform.GetRotation());
            movement_vectors.Movement_Dir = glm::vec3();

            std::cout << movement_vectors.Front.x << ", " << movement_vectors.Front.y << ", " << movement_vectors.Front.z << std::endl;
        }

        void MoveFromAxes(float xoffset, float yoffset)
        {
            glm::vec2 axes(xoffset, yoffset);

            if (axes != glm::vec2(0))
            {
                movement_vectors.Movement_Dir += movement_vectors.Front * axes.y;
                movement_vectors.Movement_Dir += glm::normalize(glm::cross(movement_vectors.Front, movement_vectors.Up)) * axes.x;
            }
        }

        void RotateFromAxes(float xoffset, float yoffset, const bool constrain_pitch = true)
        {
            auto current_yaw =
                glm::angleAxis(xoffset * (1.0f / 60.0f), glm::vec3(0, 1, 0)) *
                transform.GetRotation();
            transform.SetRotation(current_yaw);
        }

        void MoveForward()
        {
            movement_vectors.Movement_Dir += movement_vectors.Front;
        }

        void MoveBackward()
        {
            movement_vectors.Movement_Dir -= movement_vectors.Front;
        }

        void MoveLeft()
        {
            movement_vectors.Movement_Dir -= movement_vectors.Right;
        }

        void MoveRight()
        {
            movement_vectors.Movement_Dir += movement_vectors.Right;
        }
    };
}