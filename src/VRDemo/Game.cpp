#include "Game.h"
#include <iostream>

namespace Engine
{
    void Game::GameLoop()
    {
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
		physics_engine.StepSimulation(delta_time);
        window.Update();
    }

    void Game::Render()
    {
        window.SwapBuffer();
    }

    void Game::Quit(const ExitCode exit_code)
    {
        this->exit_code = exit_code;

        switch (exit_code)
        {
            case Success: std::cout << "Exiting game." << std::endl;
            case Error: std::cout << "Exiting game, error handled successfully." << std::endl;
        } 
        
        running = false;
    }

    Game::Game()
        : window(1280, 720, "VRDemo"),
        rendering_engine(vr_system),
        vr_system(std::make_shared<VRSystem>())
    {
        vr_system->CreateSessionFail.Connect([&]() 
        {
            Quit(Error);
        });

        vr_system->InitialiseFail.Connect([&]()
        {
            Quit(Error);
        });

        window.OnClose.Connect([&]()
        {
            Quit(Success);
        });

		vr_system->Init();
    }

    int Game::Run()
    {
        //start game & run game loop
        GameLoop();
		physics_engine.SetGravity(0, -10.0f, 0);
		physics_engine.SetMaxSubsteps(10);
        return exit_code;
    }
}
