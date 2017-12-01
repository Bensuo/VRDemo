/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: This code was developed as part of our Computer Games Tech Project
*/
#ifndef PHYSICS_MOTION_STATE_HPP
#define PHYSICS_MOTION_STATE_HPP
#include <LinearMath/btMotionState.h>
#include "Transform3D.hpp"

class PhysicsMotionState : public btMotionState
{
protected:
	//Some kind of game object class or something, to set the transform of our game objects
	Transform3D* m_object_transform;
	std::string name;
public:
	PhysicsMotionState(Transform3D *transform, std::string name)
		: m_object_transform(transform),
		name(name)
	{
		
	}
	~PhysicsMotionState()
	{
		
	}


	void getWorldTransform(btTransform &worldTrans) const override
	{
		auto pos = m_object_transform->GetPosition();
		auto rot = m_object_transform->GetRotation();
		worldTrans = btTransform(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z));
	}
	void setWorldTransform(const btTransform &worldTrans) override
	{

		if (m_object_transform->SyncPosition())
		{
			auto pos = worldTrans.getOrigin();
			m_object_transform->SetPosition(glm::vec3(pos.getX(), pos.getY(), pos.getZ()));
		}
		if (m_object_transform->SyncRotation())
		{
			auto rot = worldTrans.getRotation();
			m_object_transform->SetRotation(glm::quat(rot.getX(), rot.getY(), rot.getZ(), rot.getW()));
		}

	}
};


#endif // PHYSICS_MOTION_STATE_HPP
