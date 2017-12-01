#ifndef TRANSFORM_3D_HPP
#define TRANSFORM_3D_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*/

class Transform3D
{
public:
	Transform3D(glm::vec3 pos = glm::vec3(0),
		glm::vec3 scale = glm::vec3(1.0f),
		glm::quat rotation = glm::quat())
		: m_position(pos),
		m_scale(scale),
		m_rotation(rotation),
		sync_rotation(true),
		sync_position(true)
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

    glm::vec3 GetFrontDirection() const
    {
        return m_rotation * glm::vec3(0, 0, -1);
    }
	void SetSyncRotation(bool val)
	{
		sync_rotation = val;
	}
	bool SyncRotation() const
	{
		return sync_rotation;
	}
	void SetSyncPosition(bool val)
	{
		sync_position = val;
	}
	bool SyncPosition() const
	{
		return sync_position;
	}
private:
	bool sync_position;
	bool sync_rotation;
	glm::vec3 m_position;
	glm::vec3 m_scale;
	glm::quat m_rotation;
};

#endif // TRANSFORM_3D_HPP
