#include "PhysicsEngine.hpp"

Physics::PhysicsEngine::PhysicsEngine()
{
	m_broadphase = new btDbvtBroadphase();
	m_collision_configuration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collision_configuration);
	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamics_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collision_configuration);
	//Setting a sensible default value for max_substeps
	m_max_substeps = 7;
}
Physics::PhysicsEngine::~PhysicsEngine()
{
	delete m_dynamics_world;
	delete m_solver;
	delete m_dispatcher;
	delete m_collision_configuration;
	delete m_broadphase;
	
	
	
	
}

void Physics::PhysicsEngine::SetGravity(float x, float y, float z)
{
	m_dynamics_world->setGravity(btVector3(x, y, z));
}

void Physics::PhysicsEngine::AddRigidBody(RigidBody& rigid_body)
{
	
	m_dynamics_world->addRigidBody(rigid_body.m_rigid_body);
	//m_dynamics_world->updateSingleAabb(rigid_body.m_rigid_body);
	
}

void Physics::PhysicsEngine::StepSimulation(float timeStep)
{
	m_dynamics_world->stepSimulation(timeStep, m_max_substeps);
}

void Physics::PhysicsEngine::RemoveRigidBody(RigidBody &rigid_body)
{
	m_dynamics_world->removeRigidBody(rigid_body.m_rigid_body);
}

void Physics::PhysicsEngine::SetMaxSubsteps(int val)
{
	m_max_substeps = val;
}
