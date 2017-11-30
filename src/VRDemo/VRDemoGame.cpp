#include "VRDemoGame.h"
#include "Shader.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Light.h"

void VRDemoGame::HandleInput()
{
    const auto& keyboard_state = Input::Keyboard::GetState();
    const auto& mouse_state = Input::Mouse::GetState();

    if (keyboard_state.IsKeyDown(Key_Escape))
    {
        Quit();
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
        dining_room.Draw(rendering_engine);
    }
    rendering_engine.End();
}

void VRDemoGame::RenderScene(const Rendering::Shader& shader, int eye)
{
    point_light.Position = player.GetPlayerTransform().GetPosition();
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

        dining_room.Draw(rendering_engine);
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
    point_light.Position = player.GetPlayerTransform().GetPosition();
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
    : dining_room(content.LoadModel("res/models/sponza2/sponza.obj")),
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
    player(vr_system)
{
    Transform3D transform;
    transform.SetPosition(glm::vec3(0, 2.5, 0));
    player.SetTransform(transform);

    SetUpLighting();

    glm::vec3 pos(-2.0f, 0.0f, 2.0f);
    glm::vec3 scale(1.0f);
    glm::quat rot;
    dining_room.SetTransform(Transform3D(pos, scale, rot));

    shadow_shader.Use();
    shadow_shader.SetInt("shadowMap", 30);
}