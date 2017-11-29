#ifndef TRANSFORM_3D_HPP
#define TRANSFORM_3D_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform3D
{
public:
	Transform3D(glm::vec3 pos = glm::vec3(0),
		glm::vec3 scale = glm::vec3(1.0f),
		glm::quat rotation = glm::quat())
		: m_position(pos),
		m_scale(scale),
		m_rotation(rotation)
	{
		
	}
	const glm::vec3& GetPosition() const
	{
		return m_position;
	}
	const glm::vec3& GetScale() const
	{
		return m_scale;
	}
	const glm::quat& GetRotation() const
	{
		return m_rotation;
	}
	void SetPosition(const glm::vec3& new_pos)
	{
		m_position = new_pos;
	}
	void SetScale(const glm::vec3& new_scale)
	{
		m_scale = new_scale;
	}
	void SetRotation(const glm::quat& new_rotation)
	{
		m_rotation = new_rotation;
	}
private:
	glm::vec3 m_position;
	glm::vec3 m_scale;
	glm::quat m_rotation;
};

#endif // TRANSFORM_3D_HPP
