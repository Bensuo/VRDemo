#ifndef CAMERA_H
#define CAMERA_H

#include <glm/gtc/matrix_transform.hpp>
#include "GameTimer.h"
#include "VRSystem.h"

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
                glm::vec3 new_front;
                new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                new_front.y = sin(glm::radians(pitch));
                new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                front = normalize(new_front);
                right = normalize(cross(front, world_up));
                up = normalize(cross(right, front));
            }

        public:
            const glm::mat4& GetProjection() const
            {
                return projection;
            }

            const glm::vec3& GetPosition() const
            {
                return position;
            }

            explicit Camera(
                const glm::mat4& projection,
                const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
                const glm::vec3& world_up = glm::vec3(0.0f, 1.0f, 0.0f),
                const float yaw = 90.0f,
                const float pitch = 23.33f,
                const float mouse_sensitivity = 0.1f,
                const float movement_speed = 1.5f,
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
            }

            glm::mat4 GetView() const
            {
                return lookAt(position, position + front, up);
            }

			glm::mat4 GetViewVR(VRSystem& vr_system, int eye)
			{
				return vr_system.GetViewFromEye(position, eye, front);
			}

            void MoveForward()
            {
                movement += front;
            }

            void MoveBackward()
            {
                movement -= front;
            }

            void MoveLeft()
            {
                movement -= right;
            }

            void MoveRight()
            {
                movement += right;
            }

            void Update(const GameTime delta_time)
            {
                const auto speed = movement_speed * delta_time;

                if (movement != glm::vec3(0))
                {
                    movement = normalize(movement);
                }

                const auto velocity = movement * speed;

                position += velocity;

                movement = glm::vec3();
            }

            void ProcesMotion(float xoffset, float yoffset, const GLboolean constrain_pitch = true)
            {
                xoffset *= mouse_sensitivity;
                yoffset *= mouse_sensitivity;

                yaw += xoffset;
                pitch += yoffset;

                if (constrain_pitch)
                {
                    if (pitch > 89.0f)
                        pitch = 89.0f;
                    if (pitch < -89.0f)
                        pitch = -89.0f;
                }

                UpdateCameraVectors();
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
			void UpdateFromHMD(VRSystem& vr_system)
            {
	            
            }
        };
    }
}
#endif