/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: This code was developed as part of our Computer Games Tech Project
*/

#ifndef RIGID_BODY_HPP
#define RIGID_BODY_HPP
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
//Physics Engine forward declaration
namespace Physics {
	class PhysicsEngine;
}

class RigidBody
{
	friend Physics::PhysicsEngine;
public:
	btRigidBody* m_rigid_body;
	 ~RigidBody()
	 {
		 delete m_rigid_body->getMotionState();
		 delete m_rigid_body;
	 }
	 void SetLinearVelocity(float x, float y, float z)
	 {
		 m_rigid_body->setLinearVelocity(btVector3(x, y, z));
	 }
	 glm::vec3 GetLinearVelocity()
	 {
		 auto vel = m_rigid_body->getLinearVelocity();
		 return glm::vec3(vel.getX(), vel.getY(), vel.getZ());
	 }
	 void ApplyCentralForce(float x, float y, float z)
	 {
		 m_rigid_body->applyCentralForce(btVector3(x, y, z));
	 }
	 void SetRestitution(float val)
	 {
		 m_rigid_body->setRestitution(val);
	 }
	void SetDamping(float lin, float angular)
	 {
		m_rigid_body->setDamping(lin, angular);
	 }
	void SetUserPointer(void* userPointer)
	 {
		m_rigid_body->setUserPointer(userPointer);
	 }
	void Activate()
	 {
		m_rigid_body->activate();
	 }
protected:
	//Protected constructor to prevent instantiation except by derived classes
	RigidBody(){}
	
};


#endif // RIGID_BODY_HPP
