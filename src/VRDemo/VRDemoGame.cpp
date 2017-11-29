#include "VRDemoGame.h"
#include "Shader.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Light.h"
#include "RigidBodySphere.hpp"


void VRDemoGame::HandleInput()
{
    const auto& keyboard_state = Input::Keyboard::GetState();
    const auto& mouse_state = Input::Mouse::GetState();
    const auto vr_input_state = vr_system.GetInputState();
    const auto& left_axis = vr_input_state.GetLeft().ThumbstickAxis;
    const auto& right_axis = vr_input_state.GetRight().ThumbstickAxis;

    if (keyboard_state.IsKeyDown(Key_Escape))
    {
        Quit();
    }

    if (keyboard_state.IsKeyDown(Key_W))
    {
        camera.MoveForward();
    }

    if (keyboard_state.IsKeyDown(Key_A))
    {
        camera.MoveLeft();
    }

    if (keyboard_state.IsKeyDown(Key_S))
    {
        camera.MoveBackward();
    }

    if (keyboard_state.IsKeyDown(Key_D))
    {
        camera.MoveRight();
    }

    if (keyboard_state.IsKeyDown(Key_Space))
    {
        lamps_active.Down();
    }
    else
    {
        lamps_active.Up();
    }

    if (keyboard_state.IsKeyDown(Key_F))
    {
        flash_light_active.Down();
    }
    else
    {
        flash_light_active.Up();
    }

    if (keyboard_state.IsKeyDown(Key_1))
    {
        lighting_active.Down();
    }
    else
    {
        lighting_active.Up();
    }

    if (keyboard_state.IsKeyDown(Key_2))
    {
        blinn_phong.Down();
    }
    else
    {
        blinn_phong.Up();
    }

    if (keyboard_state.IsKeyDown(Key_3))
    {
        show_normal_mapping.Down();
    }
    else
    {
        show_normal_mapping.Up();
    }

    const auto& motion = mouse_state.RelativeMotion();
    camera.RotateFromAxes(right_axis.x, right_axis.y);

    camera.MoveFromAxes(left_axis.x, left_axis.y);
	hand_left->transform->SetPosition(glm::vec3( glm::vec4(vr_input_state.GetLeft().Transform.GetPosition(), 1)*camera.Yaw() ) + camera.Position());
	hand_left->rigid_body->Activate();
	hand_right->transform->SetPosition(glm::vec3(glm::vec4(vr_input_state.GetRight().Transform.GetPosition(), 1)*camera.Yaw()) + camera.Position());
}

void VRDemoGame::Update(const GameTime delta_time)
{
    HandleInput();
    //camera.Update(delta_time);

    vr_system.UpdateAvatar(delta_time);

    Game::Update(delta_time);
}

void VRDemoGame::RenderScene(const Rendering::Shader& shader, int eye)
{
    // move oculus data into our virtual camera's space
    auto new_view = glm::translate(view, vr_system.EyePos(eye));
    new_view = vr_system.GetViewFromEye(eye) * view;
    auto eye_pos = glm::vec3(glm::vec4(vr_system.EyePos(eye), 1) * view) - camera.Position();
    auto hand_left_pos = glm::vec3(glm::vec4(vr_system.GetInputState().GetLeft().Transform.GetPosition(), 1) * view) + camera.Position();
    auto hand_left_dir = glm::vec3(glm::vec4(vr_system.GetInputState().GetLeft().Transform.GetRotation() * glm::vec3(0, 0, -1), 0) * view);
    auto hand_right_pos = glm::vec3(glm::vec4(vr_system.GetInputState().GetRight().Transform.GetPosition(), 1) * view) + camera.Position();

    camera.SetFront(glm::vec3(glm::vec4(vr_system.GetFront(), 0) * view));

    point_light.Position = eye_pos;
    flash_light.Position = hand_left_pos;
    flash_light.Direction = hand_left_dir;

	rendering_engine.Begin(new_view, vr_system.GetProjectionMatrix(eye), eye_pos, shader);
    {
        shader.SetBool("blinn_phong", blinn_phong);
        shader.SetBool("lamps_active", lamps_active);
        shader.SetBool("flash_light_active", flash_light_active);
        shader.SetBool("lighting_active", lighting_active);
        shader.SetBool("show_normal_mapping", show_normal_mapping);

        rendering_engine.AddLight(flash_light);
        rendering_engine.AddLight(point_light);
        rendering_engine.AddLights(spot_lights);
        rendering_engine.AddLight(directional_light);

	    for (auto object : game_objects)
	    {
			object->Draw(rendering_engine);
	    }

    }
    rendering_engine.End();
}

void VRDemoGame::RenderSkybox(const Rendering::Shader& shader, int eye)
{
    auto new_view = glm::translate(view, vr_system.EyePos(eye));
    new_view = vr_system.GetViewFromEye(eye) * view;
    auto eye_pos = glm::vec3(glm::vec4(vr_system.EyePos(eye), 1) * view);

    rendering_engine.Begin(new_view, vr_system.GetProjectionMatrix(eye), eye_pos, shader);
    {
        skybox.Draw(rendering_engine);
    }
    rendering_engine.End();
}

void VRDemoGame::Render()
{
    view = glm::mat4_cast(camera.Yaw());
    view = glm::translate(view, -camera.Position());

    rendering_engine.ClearScreen();
	rendering_engine.BeginRender();
	for (int i = 0; i < 2; ++i)
	{
		rendering_engine.ClearEyeBuffer(i);
		RenderSkybox(skybox_shader, i);
		RenderScene(blinn_shader, i);
        vr_system.DrawAvatar(vr_system.GetViewFromEye(i), vr_system.GetProjectionMatrix(i), vr_system.EyePos(i));
		rendering_engine.Commit(i);
	}
	rendering_engine.EndRender();

    Game::Render();
}

void VRDemoGame::SetUpLighting()
{
    point_light.Position = camera.Position();
    point_light.Ambient = glm::vec3(0.1, 0.1, 0.1);
    point_light.Diffuse = glm::vec3(1.0, 1.0, 0.5);
    point_light.Specular = glm::vec3(1.0, 1.0, 1.0);
    point_light.Constant = 1.0;
    point_light.Linear = 0.09;
    point_light.Quadratic = 0.16;

    flash_light.Ambient = glm::vec3(0.0, 0.0, 0.0);
    flash_light.Diffuse = glm::vec3(1.0, 1.0, 1.0);
    flash_light.Specular = glm::vec3(1.0, 1.0, 1.0);
    flash_light.Constant = 1.0;
    flash_light.Linear = 0.2;
    flash_light.Quadratic = 0.032;
    flash_light.CutOff = glm::cos(glm::radians(15.0f));
    flash_light.OuterCutOff = glm::cos(glm::radians(25.0f));
    flash_light.Position = camera.Position();
    flash_light.Direction = camera.GetFront();

    directional_light.Direction = glm::vec3(-1.0, -1.0, -0.666);
    directional_light.Ambient = glm::vec3(0.0, 0.0, 0.0);
    directional_light.Diffuse = glm::vec3(0.1, 0.2, 0.4);
    directional_light.Specular = glm::vec3(1.0, 1.0, 1.0);

    Rendering::SpotLight spot_light;
    spot_light.Ambient = glm::vec3(0.0, 0.0, 0.0);
    spot_light.Diffuse = glm::vec3(1.0, 1.0, 0.5);
    spot_light.Specular = glm::vec3(1.0, 1.0, 1.0);
    spot_light.Constant = 1.0;
    spot_light.Linear = 0.09;
    spot_light.Quadratic = 0.032;
    spot_light.CutOff = glm::cos(glm::radians(32.0));
    spot_light.OuterCutOff = glm::cos(glm::radians(84.0));
    spot_light.Position = glm::vec3(-2.5, 4.0, -2.0);
    spot_light.Direction = glm::vec3(0.0, -1.0, 0.0);

    Rendering::SpotLight spot_light2;
    spot_light2.Ambient = glm::vec3(0.0, 0.0, 0.0);
    spot_light2.Diffuse = glm::vec3(1.0, 1.0, 0.5);
    spot_light2.Specular = glm::vec3(1.0, 1.0, 1.0);
    spot_light2.Constant = 1.0;
    spot_light2.Linear = 0.09;
    spot_light2.Quadratic = 0.032;
    spot_light2.CutOff = glm::cos(glm::radians(32.0));
    spot_light2.OuterCutOff = glm::cos(glm::radians(84.0));
    spot_light2.Position = glm::vec3(1.0, 4.0, -2.0);
    spot_light2.Direction = glm::vec3(0.0, -1.0, 0.0);

    spot_lights.emplace_back(spot_light);
    spot_lights.emplace_back(spot_light2);
}

VRDemoGame::VRDemoGame()
	: camera(glm::perspective(1.0f, 1280.0f / 720.0f, 0.1f, 100.0f),
		glm::vec3(-0.6, 0.0f, 5),
		glm::vec3(0.0, 1.0, 0.0),
		-90.0f,
		-23.33f),
	//dining_room(content.LoadModel("res/models/sponza2/sponza.obj")),
	test_hands(content.LoadModel("res/models/cube.obj")),
	skybox(content.LoadSkybox("res/textures/right.bmp",
		"res/textures/left.bmp",
		"res/textures/top.bmp",
		"res/textures/bottom.bmp",
		"res/textures/back.bmp",
		"res/textures/front.bmp")),
	//sponza_mesh(new RigidBodyMesh(content.LoadCollisionMesh("res/models/sponza2/sponza.obj"), glm::vec3(1.0f), glm::vec3(0))),
	blinn_shader(content.LoadShader("res/shaders/parallax-blinn-2.vs", "res/shaders/parallax-blinn-2.fs")),
	skybox_shader(content.LoadShader("res/shaders/skybox.vs", "res/shaders/skybox.fs")),
	lamps_active(true),
	blinn_phong(true),
	lighting_active(true),
	show_normal_mapping(true)
{
	SetUpLighting();

	//add sponza
	game_objects.emplace_back(new GameObject());
	game_objects[0]->model = content.LoadModel("res/models/sponza2/sponza.obj");
	game_objects[0]->transform->SetScale(glm::vec3(3.0f));
	game_objects[0]->rigid_body = new RigidBodyMesh(content.LoadCollisionMesh("res/models/sponza2/sponza.obj"), glm::vec3(3.0f), glm::vec3(0));
	game_objects[0]->rigid_body->SetRestitution(1.0f);
	physics_engine.AddRigidBody(*(game_objects[0]->rigid_body), COL_SCENE, COL_OBJECTS);
	physics_engine.AddRigidBody(*camera.rigid_body, COL_OBJECTS, COL_SCENE | COL_OBJECTS);
	for (size_t i = 1; i < 50; i++)
	{
		GameObject* gameObject = new GameObject();
		//GameObject& gameObject = game_objects[i];
		gameObject->model = content.LoadModel("res/models/testcube/testsphere.obj");
		gameObject->transform->SetPosition(glm::vec3(0.0f, 100, 0));
		gameObject->transform->SetScale(glm::vec3(0.5f + (0.5f/50)*i));
		gameObject->id = "sphere";
		gameObject->is_destructable = i % 2;
		gameObject->rigid_body = new RigidBodySphere(1.0f, 2.0f, gameObject->transform);
		gameObject->rigid_body->SetRestitution(0.7f);
		gameObject->rigid_body->SetDamping(0.1f, 0.2f);
		gameObject->rigid_body->SetUserPointer(static_cast<void*>(gameObject));
		physics_engine.AddRigidBody(*(gameObject->rigid_body), COL_OBJECTS, COL_SCENE | COL_OBJECTS | COL_HANDS);
		game_objects.push_back(gameObject);
	}
	physics_engine.SetInternalTickCallback(PhysicsCallback, static_cast<void*>(this));
	hand_left = new GameObject();
	hand_left->id = "hands";
	hand_left->transform->SetPosition(camera.Yaw() * vr_system.GetInputState().GetLeft().Transform.GetPosition() + camera.Position());
	hand_left->transform->SetScale(glm::vec3(0.1f));
	hand_left->rigid_body = new KinematicSphere(1.0f,hand_left->transform);
	hand_left->rigid_body->SetUserPointer(static_cast<void*>(hand_left));

	physics_engine.AddRigidBody(*hand_left->rigid_body, COL_HANDS, COL_OBJECTS);
	hand_right = new GameObject();
	hand_right->id = "hands";
	hand_right->transform->SetPosition(camera.Yaw() * vr_system.GetInputState().GetLeft().Transform.GetPosition() + camera.Position());
	hand_right->transform->SetScale(glm::vec3(0.1f));
	hand_right->rigid_body = new KinematicSphere(1.0f, hand_right->transform);
	hand_right->rigid_body->SetUserPointer(static_cast<void*>(hand_right));

	physics_engine.AddRigidBody(*hand_right->rigid_body, COL_HANDS, COL_OBJECTS);

		//dining_room.GetTransform().SetScale(glm::vec3(0.015f));
}

void VRDemoGame::PhysicsCallback(btDynamicsWorld *world, btScalar timestep)
{
	//Limit the camera speed
	VRDemoGame* game = static_cast<VRDemoGame*>(world->getWorldUserInfo());
	//cap the camera speed
	auto vel = game->camera.rigid_body->GetLinearVelocity();
	btVector3 velocity = btVector3(vel.x, vel.y, vel.z);
	glm::vec2 v2{ velocity.getX(), velocity.getZ() };
	btScalar speed = glm::length(v2);
	if (speed > 5.0f)
	{
		v2 *= 5.0f / speed;
		game->camera.rigid_body->SetLinearVelocity(v2.x, velocity.getY(), v2.y);
	}

	//Find contacts between hands and spheres and delete as appropriate
	std::vector<GameObject*> for_deletion;
	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();
		GameObject* ob_a = static_cast<GameObject*>(obA->getUserPointer());
		GameObject* ob_b = static_cast<GameObject*>(obB->getUserPointer());
		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				if (ob_a && ob_b)
				{
					if (ob_b->id == "hands" && ob_a->id == "sphere" && ob_a->is_destructable)
					{
						for_deletion.push_back(ob_a);
					}
				}
			}
		}

	}
	for (int i = 0; i < for_deletion.size(); ++i)
	{
		
		for (int j = game->game_objects.size()-1; j > 0;  --j)
		{
			if (game->game_objects[j] == for_deletion[i])
			{
				world->removeRigidBody(for_deletion[i]->rigid_body->m_rigid_body);
				delete game->game_objects[j];
				game->game_objects.erase(game->game_objects.begin() + j);
				break;
			}
		}
	}

}
