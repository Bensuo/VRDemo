#ifndef BLINNPHONGGAME_H
#define BLINNPHONGGAME_H

#include "Player.h"
#include "Game.h"
#include "Skybox.h"
#include "Light.h"
#include "Button.h"
#include <OVR_CAPI_GL.h>
using namespace Engine;
/**
 * \brief Our top-level class. Contains the scene and allows the player to interact with it.
 */
class VRDemoGame : public Game
{
    VRPlayer player;
    Rendering::Model dining_room;
    Rendering::Skybox skybox;

    Rendering::Shader shadow_shader;
    Rendering::Shader blinn_shader;
    Rendering::Shader skybox_shader;
	Rendering::Shader textured_shader;

    Rendering::SpotLight flash_light;
    Rendering::DirectionalLight directional_light;
    Rendering::PointLight point_light;

    std::vector<Rendering::SpotLight> spot_lights;

    Input::Button lamps_active;
    Input::Button blinn_phong;
    Input::Button flash_light_active;
    Input::Button lighting_active;
    Input::Button show_normal_mapping;

    glm::mat4 light_space_matrix;

    /**
     * \brief Render a skybox.
     * \param shader The shader you want to use while rendering the skybox.
     */
    void RenderSkybox(const Rendering::Shader& shader, int eye);

    /**
     * \brief Render the game.
     */
    void Render() override;

    /**
     * \brief Set up the scene's lighting
     */
    void SetUpLighting();

    /**
     * \brief Check input devices and update game state
     */
    void HandleInput();

    /**
     * \brief Update the game.
     * \param delta_time The time between the last two timesteps.
     */

    void Update(const GameTime delta_time) override;

    void RenderScene(const Rendering::Shader & shader);

    /**
     * \brief Render the scene.
     * \param shader The shader you want to use while rendering the scene.
     */
    void RenderScene(const Rendering::Shader& shader, int eye);
public:
    /**
     * \brief Load the game's content & set up the scene.
     */
    VRDemoGame();
};

#endif // BLINNPHONGGAME_H