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
    camera.ProcesMotion(motion.x, motion.y);
}

void VRDemoGame::Update(const GameTime delta_time)
{
    HandleInput();
    camera.Update(delta_time);

    Game::Update(delta_time);
}

void VRDemoGame::RenderScene(const Rendering::Shader& shader, int eye)
{
    point_light.Position = camera.GetPosition();
    flash_light.Position = camera.GetPosition();
    flash_light.Direction = camera.GetFront();

	rendering_engine.Begin(camera.GetViewVR(vr_system, eye), vr_system.GetProjectionMatrix(eye), camera.GetPosition(), shader);
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

        dining_room.Draw(rendering_engine);
    }
    rendering_engine.End();
}

void VRDemoGame::RenderSkybox(const Rendering::Shader& shader, int eye)
{
    rendering_engine.Begin(camera.GetViewVR(vr_system, eye), vr_system.GetProjectionMatrix(eye), camera.GetPosition(), shader);
    {
        skybox.Draw(rendering_engine);
    }
    rendering_engine.End();
}

void VRDemoGame::Render()
{
    rendering_engine.ClearScreen();
	rendering_engine.BeginRender();
	for (int i = 0; i < 2; ++i)
	{
		rendering_engine.ClearEyeBuffer(i);
		RenderSkybox(skybox_shader, i);
		RenderScene(blinn_shader, i);
		rendering_engine.Commit(i);
	}
	rendering_engine.EndRender();

    Game::Render();
}

void VRDemoGame::SetUpLighting()
{
    point_light.Position = camera.GetPosition();
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
    flash_light.Position = camera.GetPosition();
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
        glm::vec3(-0.6, 5, 5),
        glm::vec3(0.0, 1.0, 0.0),
        -90.0f,
        -23.33f),
    dining_room(content.LoadModel("res/models/breakfast_room.obj")),
    skybox(content.LoadSkybox("res/textures/right.bmp",
        "res/textures/left.bmp",
        "res/textures/top.bmp",
        "res/textures/bottom.bmp",
        "res/textures/back.bmp",
        "res/textures/front.bmp")),
    blinn_shader(content.LoadShader("res/shaders/blinn-phong.vs", "res/shaders/blinn-phong.fs")),
    skybox_shader(content.LoadShader("res/shaders/skybox.vs", "res/shaders/skybox.fs")),
    lamps_active(true),
    blinn_phong(true),
    lighting_active(true),
    show_normal_mapping(true)
{
    SetUpLighting();
	
}