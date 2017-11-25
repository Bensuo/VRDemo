#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp> 

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
            const glm::mat4& Projection() const
            {
                return projection;
            }

            const glm::vec3& Position() const
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
                const float movement_speed = 20.0f,
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

            glm::mat4 View() const
            {
                return lookAt(position, position + front, up);
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

            void Update(const float delta_time)
            {
                const auto speed = movement_speed * delta_time;

                const auto velocity = movement * speed;

                position += velocity;

                movement = glm::vec3();
            }

            void MoveFromAxes(float xoffset, float yoffset)
            {
                glm::vec2 axes(xoffset, -yoffset);

                if (axes != glm::vec2(0))
                {
                    movement += front * axes.y;
                    movement += glm::normalize(glm::cross(front, up)) * axes.x;
                }
            }

            void RotateFromAxes(float xoffset, float yoffset, const bool constrain_pitch = true)
            {
                yaw += xoffset * 3;
                pitch -= yoffset * 3;

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