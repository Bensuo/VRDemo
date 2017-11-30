#pragma once

#include "VRSystem.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include "RigidBody.hpp"
#include "RigidBodySphere.hpp"

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
        Transform3D* transform;
        MovementVectors movement_vectors;
		RigidBody* rigid_body;
		glm::vec3 rigid_body_offset;
		float movement_speed = 2000.0f;
    public:
        VRPlayer(std::shared_ptr<VRSystem> system, const glm::vec3& initial_pos)
            : vr_system(system), input_state()
        {
			transform = new Transform3D(initial_pos, glm::vec3(0.1f));
			transform->SetSyncRotation(false);
            movement_vectors.World_up = glm::vec3(0, 1, 0);
            movement_vectors.Update(vr_system->GetHMDTransform().GetFrontDirection() * transform->GetRotation());
			rigid_body = new RigidBodySphere(1.0f, 100.0f, transform);
			rigid_body->SetRestitution(1.0f);
			rigid_body->m_rigid_body->setDamping(0.99f, 0.99f);

        }

		RigidBody* GetRigidBody()
        {
			return rigid_body;
        }

        Transform3D* GetPlayerTransform()
        {
            return transform;
        }

        glm::mat4 GetEyeViewWorldSpace(int eye)
        {
            auto view = glm::mat4_cast(transform->GetRotation());
            view = glm::translate(view, -transform->GetPosition());
            return vr_system->GetViewFromEye(eye) * view;
        }

        void SetTransform(Transform3D* transform)
        {
            this->transform = transform;
            movement_vectors.Update(vr_system->GetHMDTransform().GetFrontDirection() * transform->GetRotation());
        }

        glm::vec3 GetEyePosWorldspace(int eye)
        {
            auto rot = glm::mat4_cast(transform->GetRotation());
            return glm::vec3(glm::vec4(vr_system->GetEyePos(eye), 1) * glm::mat4()) + transform->GetPosition();
        }

        glm::vec3 GetLeftHandDirWorldspace()
        {
            return vr_system->GetInputState().GetLeft().Transform.GetFrontDirection() * transform->GetRotation();
        }

        glm::vec3 GetLeftHandPosWorldspace()
        {
            auto rot = glm::mat4_cast(transform->GetRotation());
            rot = glm::translate(rot, -transform->GetPosition());
            return glm::vec3(glm::vec4(input_state.GetLeft().Transform.GetPosition(), 1) * rot) + transform->GetPosition();
        }

        glm::vec3 GetRightHandDirWorldspace()
        {
            return vr_system->GetInputState().GetRight().Transform.GetFrontDirection() * transform->GetRotation();
        }

        glm::vec3 GetRightHandPosWorldspace()
        {
            auto rot = glm::mat4_cast(transform->GetRotation());
            rot = glm::translate(rot, -transform->GetPosition());
            return glm::vec3(glm::vec4(input_state.GetRight().Transform.GetPosition(), 1) * rot) + transform->GetPosition();
        }

        void Update(const GameTime delta_time)
        {
            input_state = vr_system->GetInputState();

            //const auto velocity = movement_vectors.Movement_Dir * delta_time;

            //auto new_pos = transform.GetPosition() + velocity;
           //transform.SetPosition(new_pos);

            movement_vectors.Update(vr_system->GetHMDTransform().GetFrontDirection() * transform->GetRotation());
            movement_vectors.Movement_Dir = glm::vec3();
        }

        void MoveFromAxes(float xoffset, float yoffset)
        {
            glm::vec2 axes(xoffset, yoffset);

            if (axes != glm::vec2(0))
            {
				rigid_body->Activate();
				glm::vec3 new_front = movement_vectors.Front * axes.y * movement_speed;
				rigid_body->ApplyCentralForce(new_front.x, new_front.y, new_front.z);
				glm::vec3 new_right = movement_vectors.Right * axes.x * movement_speed;
				rigid_body->ApplyCentralForce(new_right.x, new_right.y, new_right.z);
            }
        }

        void RotateFromAxes(float xoffset, float yoffset, const bool constrain_pitch = true)
        {
            auto current_yaw =
                glm::angleAxis(xoffset * (1.0f / 60.0f), glm::vec3(0, 1, 0)) *
                transform->GetRotation();
            transform->SetRotation(current_yaw);
        }

        void MoveForward()
        {
            //movement_vectors.Movement_Dir += movement_vectors.Front;
			auto new_front = movement_vectors.Front * movement_speed;
			rigid_body->Activate();
			rigid_body->ApplyCentralForce(new_front.x, new_front.y, new_front.z);
        }

        void MoveBackward()
        {
            //movement_vectors.Movement_Dir -= movement_vectors.Front;
			auto new_front = -movement_vectors.Front * movement_speed;
			rigid_body->Activate();
			rigid_body->ApplyCentralForce(new_front.x, new_front.y, new_front.z);
        }

        void MoveLeft()
        {
           //movement_vectors.Movement_Dir -= movement_vectors.Right;
			auto new_right = -movement_vectors.Right * movement_speed;
			rigid_body->Activate();
			rigid_body->ApplyCentralForce(new_right.x, new_right.y, new_right.z);
        }

        void MoveRight()
        {
            //movement_vectors.Movement_Dir += movement_vectors.Right;
			auto new_right = movement_vectors.Right * movement_speed;
			rigid_body->Activate();
			rigid_body->ApplyCentralForce(new_right.x, new_right.y, new_right.z);
        }
    };
}
