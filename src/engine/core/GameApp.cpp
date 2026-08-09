#include "GameApp.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>

#include "Time.h"

namespace engine::core
{
    GameApp::GameApp(){
        Time_ = std::make_unique<Time>();
    }

    GameApp::~GameApp()
    {
        if (IsRunning_)
        {
            spdlog::warn("Game shutdown exception: Close() was not called. Calling manually now.");
            Close();
        }
    }
    void GameApp::Run()
    {
        if (!Initalize())
        {
            spdlog::error("Initialization failed, unable to continue the game.");
            return;
        }
        Time_->SetTargetFPS(0);

        while (IsRunning_)
        {
            Time_->Update();
            auto deltaTime = Time_->GetUnscaledDeltaTime();
            HandleInput();
            Update(deltaTime);
            Render();
            spdlog::info("Delta time:{:.6f},FPS:{:.2f}",deltaTime,1.0 / deltaTime);
        }
        Close();
    }

    bool GameApp::Initalize()
    {
        spdlog::info("Initializing game.");

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        {
            spdlog::error("SDL initialization failed.");
            return false;
        }
        if (!SDL_CreateWindowAndRenderer("SunnyLand", 1280, 720, SDL_WINDOW_RESIZABLE, &Window_, &Renderer_))
        {
            spdlog::error("Failed to create SDL window and renderer,SDL Error:{}.",SDL_GetError());
            return false;
        }

        IsRunning_ = true;

        return true;
    }

    void GameApp::HandleInput(){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                IsRunning_ = false;
            }
        }
    }
    void GameApp::Render()
    {
    }

    void GameApp::Update(float /*deltaTime*/)
    {
    }

    void GameApp::Close()
    {
         spdlog::trace("Close game.");
        if (Window_ != nullptr)
        {
            SDL_DestroyWindow(Window_);
            Window_ = nullptr;
        }
        if (Renderer_ != nullptr)
        {
            SDL_DestroyRenderer(Renderer_);
            Renderer_ = nullptr;
        }
        SDL_Quit();
        IsRunning_ = false;
    }

} // namespace engine::core
