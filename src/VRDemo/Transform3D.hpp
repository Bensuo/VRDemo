#ifndef TRANSFORM_3D_HPP
#define TRANSFORM_3D_HPP

#include <glm/glm.hpp>
class Transform3D
{
public:
	Transform3D(glm::vec3 pos = glm::vec3(0),
		glm::vec3 scale = glm::vec3(0),
		glm::vec3 rotation = glm::vec3(0))
		: m_position(pos),
		m_scale(scale),
		m_rotation(rotation)
	{
		
	}
	const glm::vec3& GetPosition()
	{
		return m_position;
	}
	const glm::vec3& GetScale()
	{
		return m_scale;
	}
	const glm::vec3& GetRotation()
	{
		return m_rotation;
	}
	void SetPosition(glm::vec3& new_pos)
	{
		m_position = new_pos;
	}
	void SetScale(glm::vec3& new_scale)
	{
		m_scale = new_scale;
	}
	void SetRotation(glm::vec3& new_rotation)
	{
		m_rotation = new_rotation;
	}
private:
	glm::vec3 m_position;
	glm::vec3 m_scale;
	glm::vec3 m_rotation;
};

#endif // TRANSFORM_3D_HPP