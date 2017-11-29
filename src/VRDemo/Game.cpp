#include "Game.h"
#include <iostream>

namespace Engine
{
    void Game::GameLoop()
    {
        running = true;
        game_timer.Initialize();
        while (running)
        {
            game_timer.Reset();

            //process individual frame's worth of updates
            while (!game_timer.FrameComplete())
            {
                game_timer.Update();
                Update(game_timer.DeltaTime());
            }

            //render processed frame
            if (game_timer.FrameComplete())
            {
                Render();
                game_timer.IncrementFrames();
            }
            else
            {
                game_timer.Sleep();
            }
        }
    }

    void Game::Update(const GameTime delta_time)
    {
        window.Update();
    }

    void Game::Render()
    {
        window.SwapBuffer();
    }

    void Game::Quit()
    {
        std::cout << "Quitting BlinnPhongGame" << std::endl;
        running = false;
    }

    Game::Game()
        : window(1280, 720, "VRDemo"),
        rendering_engine(vr_system),
        vr_system(std::make_shared<VRSystem>())
    {
        window.OnClose.Connect([&]()
        {
            Quit();
        });
		vr_system->Init();
    }

    int Game::Run()
    {
        //start game & run game loop
        GameLoop();

        return 0;
    }
}
