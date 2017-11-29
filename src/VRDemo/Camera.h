#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include "RigidBody.hpp"
#include "RigidBodyBox.hpp"
#include "RigidBodySphere.hpp"

namespace Engine
{
    namespace Rendering
    {
        /**
        * \brief Basic first-person camera class. Uses the relative motion of the mouse and keyboard input to construct a view matrix.
        */
        class Camera
        {
            glm::vec3 position;
            glm::vec3 front;
            glm::vec3 up;
            glm::vec3 right;
            glm::vec3 world_up;
            glm::vec3 movement;
            glm::mat4 projection;

            float yaw;
            float pitch;

            float movement_speed;
            float mouse_sensitivity;
            float zoom;
			
            void UpdateCameraVectors()
            {
                front = normalize(front);
                right = normalize(cross(front, world_up));
                up = normalize(cross(right, front));
            }

        public:
			Transform3D* transform;
			RigidBody* rigid_body;
            const glm::mat4& Projection() const
            {
                return projection;
            }

            const glm::vec3& Position() const
            {

                return transform->GetPosition();
            }

            void SetFront(const glm::vec3& front)
            {
                this->front = front;
            }

            explicit Camera(
                const glm::mat4& projection,
                const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
                const glm::vec3& world_up = glm::vec3(0.0f, 1.0f, 0.0f),
                const float yaw = 90.0f,
                const float pitch = 23.33f,
                const float mouse_sensitivity = 0.1f,
                const float movement_speed = 2000.0f,
                const float zoom = 45.0f)
                : position(position),
                front(glm::vec3(0)),
                world_up(world_up),
                movement(glm::vec3(0)),
                projection(projection),
                yaw(yaw),
                pitch(pitch),
                movement_speed(movement_speed),
                mouse_sensitivity(mouse_sensitivity),
                zoom(zoom)
            {
                UpdateCameraVectors();
				this->position.y = 40;
				transform = new Transform3D(this->position);
				rigid_body = new RigidBodySphere(0.5f, 100.0f,transform);
				rigid_body->SetRestitution(1.0f);
				//rigid_body->m_rigid_body->setAngularFactor(btVector3(0.0f,0.0f, 0.0f));
				rigid_body->m_rigid_body->setDamping(0.99f, 0.99f);
            }

            glm::mat4 View() const
            {
                return lookAt(position, position + front, up);
            }

            void MoveForward()
            {
                //movement -= front;
				auto new_front = front * movement_speed;
				rigid_body->m_rigid_body->activate();
				rigid_body->ApplyCentralForce(new_front.x, new_front.y, new_front.z);
            }

            void MoveBackward()
            {
				auto new_front = -front * movement_speed;
				rigid_body->m_rigid_body->activate();
				rigid_body->ApplyCentralForce(new_front.x, new_front.y, new_front.z);
            }

            void MoveLeft()
            {
				auto new_right = -right * movement_speed;
				rigid_body->m_rigid_body->activate();
				rigid_body->ApplyCentralForce(new_right.x, new_right.y, new_right.z);
            }

            void MoveRight()
            {
				auto new_right = right * movement_speed;
				rigid_body->m_rigid_body->activate();
				rigid_body->ApplyCentralForce(new_right.x, new_right.y, new_right.z);
            }

            glm::quat Yaw()
            {
                return glm::angleAxis(yaw, glm::vec3(0, 1, 0));
            }

            void Update(const float delta_time)
            {
                const auto velocity = movement * delta_time;

                position += velocity;

                movement = glm::vec3();
            }

            void MoveFromAxes(float xoffset, float yoffset)
            {
                glm::vec2 axes(xoffset, yoffset);

                if (axes != glm::vec2(0))
                {
					rigid_body->m_rigid_body->activate();
					glm::vec3 new_front = front * axes.y * movement_speed;
                    rigid_body->ApplyCentralForce(new_front.x, new_front.y, new_front.z);
					glm::vec3 new_right = right * axes.x * movement_speed;
					rigid_body->ApplyCentralForce(new_right.x, new_right.y, new_right.z);
                }
            }

            void RotateFromAxes(float xoffset, float yoffset, const bool constrain_pitch = true)
            {
                yaw += xoffset * 1 / 60;
                pitch -= yoffset * 1 / 60;

                if (constrain_pitch)
                {
                    if (pitch > 89.0f)
                        pitch = 89.0f;
                    if (pitch < -89.0f)
                        pitch = -89.0f;
                }

                UpdateCameraVectors();
            }

            void SetPosition(const glm::vec3& position)
            {
                this->position = position;
            }

            void ProcessMouseScroll(const float yoffset)
            {
                if (zoom >= 1.0f && zoom <= 45.0f)
                    zoom -= yoffset;
                if (zoom <= 1.0f)
                    zoom = 1.0f;
                if (zoom >= 45.0f)
                    zoom = 45.0f;
            }

            const glm::vec3& GetFront() const
            {
                return front;
            }
        };
    }
}
#endif