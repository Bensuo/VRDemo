#ifndef GAME_H
#define GAME_H

#include "Window.h"
#include "GameTimer.h"
#include "ContentLoader.h"
#include "RenderingEngine.h"
#include <OVR_CAPI_GL.h>
namespace Engine
{
    /**
     * \brief Our base-game class. Abstracts the game loop, graphics device and windowing implementation details from subclasses.
     */
    class Game
    {
        bool running = true;

        /**
         * \brief Run the main game loop.
         */
        void GameLoop();
    protected:
        Content::ContentLoader content;
        Windowing::Window window;
		VRSystem vr_system;
        Rendering::RenderingEngine rendering_engine;
        Time::GameTimer game_timer;
		
        /**
         * \brief Exit the main game loop
         */
        void Quit();

        /**
         * \brief Update the main event loop.
         * \param delta_time The time between this timestep and the last
         */
        virtual void Update(const GameTime delta_time);

        /**
         * \brief Swap the buffer.
         */
        virtual void Render();
    public:
        Game();
        virtual ~Game() = default;

        /**
        * \brief Start the game loop
        * \return Program exit code
        */
        int Run();
    };
}
#endif // GAME_H
