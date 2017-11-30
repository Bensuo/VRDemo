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

    if (keyboard_state.IsKeyDown(Key_Escape))
    {
        Quit(Success);
    }

    if (keyboard_state.IsKeyDown(Key_W))
    {
        player.MoveForward();
    }

    if (keyboard_state.IsKeyDown(Key_A))
    {
        player.MoveLeft();
    }

    if (keyboard_state.IsKeyDown(Key_S))
    {
        player.MoveBackward();
    }

    if (keyboard_state.IsKeyDown(Key_D))
    {
        player.MoveRight();
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

    const auto& vr_input_state = vr_system->GetInputState();
    const auto& left_axis = vr_input_state.GetLeft().ThumbstickAxis;
    const auto& right_axis = vr_input_state.GetRight().ThumbstickAxis;
    player.RotateFromAxes(right_axis.x, right_axis.y);
    player.MoveFromAxes(left_axis.x, left_axis.y);
	hand_left->transform->SetPosition(player.GetLeftHandPosWorldspace());
	hand_left->rigid_body->Activate();
	hand_right->transform->SetPosition(player.GetRightHandPosWorldspace());
	hand_right->rigid_body->Activate();
}

void VRDemoGame::Update(const GameTime delta_time)
{
    player.Update(delta_time);
    HandleInput();

    vr_system->UpdateAvatar(delta_time);

    Game::Update(delta_time);
}

void VRDemoGame::RenderScene(const Rendering::Shader& shader)
{
    rendering_engine.Begin(shader);
    {
		for (auto object : game_objects)
		{
			object->Draw(rendering_engine);
		}
    }
    rendering_engine.End();
}

void VRDemoGame::RenderScene(const Rendering::Shader& shader, int eye)
{
    point_light.Position = player.GetPlayerTransform()->GetPosition();
    flash_light.Position = player.GetLeftHandPosWorldspace();
    flash_light.Direction = player.GetLeftHandDirWorldspace();

    rendering_engine.Begin(
        player.GetEyeViewWorldSpace(eye),
        vr_system->GetProjectionMatrix(eye),
        player.GetEyePosWorldspace(eye),
        shader); 
    {
        shader.SetVec3("lightPos", player.GetLeftHandPosWorldspace());
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
    rendering_engine.Begin(
        player.GetEyeViewWorldSpace(eye), 
        vr_system->GetProjectionMatrix(eye),
        player.GetEyePosWorldspace(eye), 
        shader);
    {
        skybox.Draw(rendering_engine);
    }
    rendering_engine.End();
}

void VRDemoGame::Render()
{
    auto hand_left_pos = player.GetLeftHandPosWorldspace();
    auto hand_left_dir = player.GetLeftHandDirWorldspace();

    rendering_engine.BeginDepthPass(hand_left_pos, hand_left_dir, 1.0f, 30.0f, 55.0f);
    RenderScene(rendering_engine.DepthShader());
    rendering_engine.EndDepthPass();

    rendering_engine.ClearScreen();
	rendering_engine.BeginRender();
	for (int i = 0; i < 2; ++i)
	{
		rendering_engine.ClearEyeBuffer(i);
		//RenderSkybox(skybox_shader, i);
		RenderScene(shadow_shader, i);
        //vr_system.DrawAvatar(vr_system.GetViewFromEye(i), vr_system.GetProjectionMatrix(i), vr_system.EyePos(i));
		rendering_engine.Commit(i);
	}
	rendering_engine.EndRender();

    Game::Render();
}

void VRDemoGame::SetUpLighting()
{
    point_light.Position = player.GetPlayerTransform()->GetPosition();
    point_light.Ambient = glm::vec3(0.0333, 0.0333, 0.0333);
    point_light.Diffuse = glm::vec3(1.0, 0.65, 0.45);
    point_light.Specular = glm::vec3(1.0, 1.0, 1.0);
    point_light.Constant = 1.0;
    point_light.Linear = 0.7;
    point_light.Quadratic = 1.8;

    flash_light.Ambient = glm::vec3(0.0, 0.0, 0.0);
    flash_light.Diffuse = glm::vec3(1.0, 1.0, 1.0);
    flash_light.Specular = glm::vec3(1.0, 1.0, 1.0);
    flash_light.Constant = 1.0;
    flash_light.Linear = 0.2;
    flash_light.Quadratic = 0.032;
    flash_light.CutOff = glm::cos(glm::radians(15.0f));
    flash_light.OuterCutOff = glm::cos(glm::radians(25.0f));
    flash_light.Position = player.GetLeftHandPosWorldspace();
    flash_light.Direction = player.GetLeftHandDirWorldspace();

    directional_light.Direction = glm::vec3(-1.0, -1.0, -0.666);
    directional_light.Ambient = glm::vec3(0.0, 0.0, 0.0);
    directional_light.Diffuse = glm::vec3(0.15, 0.2, 0.4);
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
    : //dining_room(content.LoadModel("res/models/sponza2/sponza.obj")),
    skybox(content.LoadSkybox("res/textures/right.bmp",
        "res/textures/left.bmp",
        "res/textures/top.bmp",
        "res/textures/bottom.bmp",
        "res/textures/back.bmp",
        "res/textures/front.bmp")),
    shadow_shader(content.LoadShader("res/shaders/parallax-blinn-2.vs", "res/shaders/parallax-blinn-2.fs")),
    skybox_shader(content.LoadShader("res/shaders/skybox.vs", "res/shaders/skybox.fs")),
    lamps_active(true),
    blinn_phong(true),
    lighting_active(true),
    show_normal_mapping(true),
    player(vr_system, glm::vec3(0, 5.0f, 0))
{


    SetUpLighting();
    //glm::vec3 pos(-2.0f, 0.0f, 2.0f);
    //glm::vec3 scale(1.0f);
    //glm::quat rot;
    //dining_room.SetTransform(Transform3D(pos, scale, rot));
    shadow_shader.Use();
    shadow_shader.SetInt("shadowMap", 30);
	game_objects.emplace_back(new GameObject());
	game_objects[0]->model = content.LoadModel("res/models/sponza2/sponza.obj");
	game_objects[0]->transform->SetScale(glm::vec3(1.2f));
	game_objects[0]->rigid_body = new RigidBodyMesh(content.LoadCollisionMesh("res/models/sponza2/sponza.obj"), glm::vec3(1.2f), glm::vec3(0));
	game_objects[0]->rigid_body->SetRestitution(1.0f);
	physics_engine.AddRigidBody(*(game_objects[0]->rigid_body), COL_SCENE, COL_OBJECTS);
	physics_engine.AddRigidBody(*player.GetRigidBody(), COL_OBJECTS, COL_SCENE | COL_OBJECTS);
	for (size_t i = 1; i < 20; i++)
	{
		GameObject* gameObject = new GameObject();
		//GameObject& gameObject = game_objects[i];
		gameObject->model = content.LoadModel("res/models/testcube/testsphere.obj");
		gameObject->transform->SetPosition(glm::vec3(0.0f + (-2.0f / 20) * i, 10 + (-3.0f/20) * i, 0 + (-2.0f / 20) * i));
		gameObject->transform->SetScale(glm::vec3(0.3f + (0.5f/20)*i));
		gameObject->id = "sphere";
		gameObject->is_destructable = i % 2;
		gameObject->rigid_body = new RigidBodySphere(1.0f, 2.0f, gameObject->transform);
		gameObject->rigid_body->SetRestitution(0.7f);
		gameObject->rigid_body->SetDamping(0.1f, 0.2f);
		gameObject->rigid_body->SetUserPointer(static_cast<void*>(gameObject));
		physics_engine.AddRigidBody(*(gameObject->rigid_body), COL_OBJECTS, COL_SCENE | COL_OBJECTS | COL_HANDS);
		float delta = 50000.0f / 20;
		gameObject->rigid_body->Activate();
		gameObject->rigid_body->ApplyCentralForce(delta*i, delta*i, delta*i);
		game_objects.push_back(gameObject);
	}
	physics_engine.SetInternalTickCallback(PhysicsCallback, static_cast<void*>(this));
	hand_left = new GameObject();
	hand_left->id = "hands";
	hand_left->transform->SetPosition(player.GetLeftHandPosWorldspace());
	hand_left->transform->SetScale(glm::vec3(0.1f));
	hand_left->rigid_body = new KinematicSphere(1.0f,hand_left->transform);
	hand_left->rigid_body->SetUserPointer(static_cast<void*>(hand_left));

	physics_engine.AddRigidBody(*hand_left->rigid_body, COL_HANDS, COL_OBJECTS);
	hand_right = new GameObject();
	hand_right->id = "hands";
	hand_right->transform->SetPosition(player.GetLeftHandPosWorldspace());
	hand_right->transform->SetScale(glm::vec3(0.1f));
	hand_right->rigid_body = new KinematicSphere(1.0f, hand_right->transform);
	hand_right->rigid_body->SetUserPointer(static_cast<void*>(hand_right));

	physics_engine.AddRigidBody(*hand_right->rigid_body, COL_HANDS, COL_OBJECTS);

}

void VRDemoGame::PhysicsCallback(btDynamicsWorld *world, btScalar timestep)
{
	//Since we are in a static method, obtain a pointer to our VRDemoGame to access game data
	VRDemoGame* game = static_cast<VRDemoGame*>(world->getWorldUserInfo());
	//cap the camera speed
	auto vel = game->player.GetRigidBody()->GetLinearVelocity();
	btVector3 velocity = btVector3(vel.x, vel.y, vel.z);
	glm::vec2 v2{ velocity.getX(), velocity.getZ() };
	btScalar speed = glm::length(v2);
	if (speed > 5.0f)
	{
		v2 *= 5.0f / speed;
		game->player.GetRigidBody()->SetLinearVelocity(v2.x, velocity.getY(), v2.y);
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
