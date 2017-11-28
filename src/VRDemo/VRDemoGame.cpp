#include "VRDemoGame.h"
#include "Shader.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Light.h"

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
}

void VRDemoGame::Update(const GameTime delta_time)
{
    HandleInput();
    camera.Update(delta_time);

    vr_system.UpdateAvatar(delta_time);

    Game::Update(delta_time);
}

void VRDemoGame::RenderScene(const Rendering::Shader& shader)
{
    rendering_engine.Begin(shader);
    {
        rendering_engine.ScaleMatrix(glm::vec3(1.0f));
        dining_room.Draw(rendering_engine);
    }
    rendering_engine.End();
}

void VRDemoGame::RenderScene(const Rendering::Shader& shader, int eye)
{
    // move oculus data into our virtual camera's space
    auto new_view = vr_system.GetViewFromEye(eye);
    auto eye_pos = glm::vec3(glm::vec4(vr_system.EyePos(eye), 1));
    auto hand_left_pos = glm::vec3(glm::vec4(vr_system.GetInputState().GetLeft().Transform.GetPosition(), 1));
    auto hand_left_dir = glm::vec3(glm::vec4(vr_system.GetInputState().GetLeft().Transform.GetRotation() * glm::vec3(0, 0, -1), 0));
    auto hand_right_pos = glm::vec3(glm::vec4(vr_system.GetInputState().GetRight().Transform.GetPosition(), 1));

    camera.SetFront(glm::vec3(glm::vec4(vr_system.GetFront(), 0)));

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

        dining_room.Draw(rendering_engine);
        test_hands.Draw(rendering_engine);
    }
    rendering_engine.End();
}

void VRDemoGame::RenderSkybox(const Rendering::Shader& shader, int eye)
{
    auto new_view = vr_system.GetViewFromEye(eye) * view;
    auto eye_pos = glm::vec3(glm::vec4(vr_system.EyePos(eye), 1) * view);

    rendering_engine.Begin(new_view, vr_system.GetProjectionMatrix(eye), eye_pos, shader);
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
    point_light.Position = camera.Position();
    point_light.Ambient = glm::vec3(0.0333, 0.0333, 0.0333);
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
    : camera(glm::perspective(1.0f, 1280.0f / 720.0f, 0.1f, 100.0f),
        glm::vec3(-0.6, 0, 5),
        glm::vec3(0.0, 1.0, 0.0),
        -90.0f,
        -23.33f),
    dining_room(content.LoadModel("res/models/sponza2/sponza.obj")),
    skybox(content.LoadSkybox("res/textures/right.bmp",
        "res/textures/left.bmp",
        "res/textures/top.bmp",
        "res/textures/bottom.bmp",
        "res/textures/back.bmp",
        "res/textures/front.bmp")),
    shadow_shader(content.LoadShader("res/shaders/parallax-blinn-2.vs", "res/shaders/parallax-blinn-2.fs")),
    depth_shader(content.LoadShader("res/shaders/shadow_depth.vs", "res/shaders/shadow_depth.fs")),
    skybox_shader(content.LoadShader("res/shaders/skybox.vs", "res/shaders/skybox.fs")),
    test_hands(content.LoadModel("res/models/cube.obj")),
    lamps_active(true),
    blinn_phong(true),
    lighting_active(true),
    show_normal_mapping(true)
{
    SetUpLighting();

    glm::vec3 pos(-2.0f, 0.0f, 2.0f);
    glm::vec3 scale(1.0f);
    glm::quat rot;
    dining_room.SetTransform(Transform3D(pos, scale, rot));

    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // shader configuration
    // --------------------
    shadow_shader.Use();
    shadow_shader.SetInt("shadowMap", 30);

    // lighting info
    // -------------

    // render loop

    // -----------
    while (true)
    {
        auto hand_left_pos = glm::vec3(glm::vec4(vr_system.GetInputState().GetLeft().Transform.GetPosition(), 1));
        auto hand_left_dir = glm::vec3(glm::vec4(vr_system.GetInputState().GetLeft().Transform.GetRotation() * glm::vec3(0, 0, -1), 0));
        test_hands.SetTransform(Transform3D(hand_left_pos, glm::vec3(0.01f), vr_system.GetInputState().GetLeft().Transform.GetRotation()));

        Update(1 / 60.0f);
        // change light position over time
        //lightPos.x = sin(glfwGetTime()) * 3.0f;
        //lightPos.z = cos(glfwGetTime()) * 2.0f;
        //lightPos.y = 5.0 + cos(glfwGetTime()) * 1.0f;

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. render depth of scene to texture (from light's perspective)
        // --------------------------------------------------------------
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 0.1f, far_plane = 20.0f;
        lightProjection = glm::perspective(glm::radians(55.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene

        lightView = glm::lookAt(hand_left_pos, hand_left_pos + hand_left_dir, glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        depth_shader.Use();
        depth_shader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        RenderScene(depth_shader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shadow_shader.Use();
        shadow_shader.SetVec3("lightPos", hand_left_pos);
        shadow_shader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE30);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        Engine::Rendering::SpotLight spot_light;
        spot_light.Ambient = glm::vec3(0.0, 0.0, 0.0);
        spot_light.Diffuse = glm::vec3(1.0, 0.65, 0.25);
        spot_light.Specular = glm::vec3(1.0, 1.0, 1.0);
        spot_light.Constant = 1.0;
        spot_light.Linear = 0.7;
        spot_light.Quadratic = 1.2;
        spot_light.CutOff = glm::cos(glm::radians(12.0f));
        spot_light.OuterCutOff = glm::cos(glm::radians(10.0f));
        spot_light.Position = hand_left_pos;
        spot_light.Direction = hand_left_dir;
        shadow_shader.SetSpotLight(spot_light);
        point_light.Position = camera.Position();

        rendering_engine.AddLight(flash_light);
        rendering_engine.AddLight(point_light);
        rendering_engine.AddLight(directional_light);

        Render();

        window.SwapBuffer();
    }
}