#pragma once
#include "Transform3D.hpp"
#include "RigidBodyBox.hpp"
#include "Model.h"

namespace Engine {namespace Rendering {
	class IRenderingEngine;
}
}

class GameObject
{
public:
	GameObject()
	{
		transform = new Transform3D();
	}
	~GameObject()
	{
		delete transform;
		delete rigid_body;
	}
	void Draw(Engine::Rendering::IRenderingEngine& rendering_engine)
	{
		rendering_engine.PushMatrix();
		rendering_engine.TranslateMatrix(transform->GetPosition());
		rendering_engine.RotateMatrix(transform->GetRotation());
		rendering_engine.ScaleMatrix(transform->GetScale());
		model.Draw(rendering_engine);
		rendering_engine.PopMatrix();
	}
	Engine::Rendering::Model model;
	Transform3D* transform;
	RigidBody* rigid_body;
private:
	
};
