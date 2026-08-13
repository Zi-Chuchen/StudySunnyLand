#include "GameApp.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>
#include "../resource/RescourceManager.h"
#include "Time.h"

namespace engine::core
{
    GameApp::GameApp() = default;

    GameApp::~GameApp()
    {
        if (IsRunning_)
        {
            spdlog::warn("Game shutdown exception: Close() was not called. Calling manually now.");
            Close();
        }
    }

    void GameApp::TestResourceManager()
    {
        spdlog::info("Testing ResourceManager.");
        if (!ResourceManager_)
        {
            spdlog::error("ResourceManager is not initialized.");
            return;
        }

        // 测试加载纹理
        SDL_Texture *texture = ResourceManager_->LoadTexture("assets/textures/Actors/eagle-attack.png.");
        if (texture)
        {
            spdlog::info("Successfully loaded texture: assets/textures/Actors/eagle-attack.png.");
            glm::vec2 size = ResourceManager_->GetTextureSize("assets/textures/Actors/eagle-attack.png.");
            spdlog::info("Texture size: {}x{}.", size.x, size.y);
        }
        else
        {
            spdlog::error("Failed to load texture: assets/textures/Actors/eagle-attack.png.");
        }

        // 测试加载音频
        MIX_Track *audio = ResourceManager_->LoadAudio("assets/audio/button_click.wav.");
        if (audio)
        {
            spdlog::info("Successfully loaded audio: assets/audio/button_click.wav.");
        }
        else
        {
            spdlog::error("Failed to load audio: assets/audio/button_click.wav.");
        }

        // 测试加载字体
        TTF_Font *font = ResourceManager_->LoadFont("assets/fonts/VonwaonBitmap-16px.ttf.", 16);
        if (font)
        {
            spdlog::info("Successfully loaded font: assets/fonts/VonwaonBitmap-16px.ttf.");
        }
        else
        {
            spdlog::error("Failed to load font: assets/fonts/VonwaonBitmap-16px.ttf.");
        }
    }

    void GameApp::Run()
    {
        if (!Initalize())
        {
            spdlog::error("Initialization failed, unable to continue the game.");
            return;
        }

        TestResourceManager();

        while (IsRunning_)
        {
            Time_->Update();
            auto deltaTime = Time_->GetUnscaledDeltaTime();
            HandleInput();
            Update(deltaTime);
            Render();
        }
        Close();
    }

    bool GameApp::Initalize()
    {
        spdlog::info("Game initialization.");

        if (!InitalizeSDL())
        {
            spdlog::error("Failed to initialize SDL.");
            return false;
        }
        if (!InitializeTime())
        {
            spdlog::error("Failed to initialize time.");
            return false;
        }

        if (!InitializeResourceManager())
        {
            spdlog::error("Failed to initialize resource manager.");
            return false;
        }

        IsRunning_ = true;

        return true;
    }

    bool GameApp::InitalizeSDL()
    {
        // 窗口初始化
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        {
            spdlog::error("Failed to initialize SDL,SDL Error:{}.", SDL_GetError());
            return false;
        }

        if (!SDL_CreateWindowAndRenderer("SunnyLand", 1280, 720, SDL_WINDOW_RESIZABLE, &Window_, &Renderer_))
        {
            spdlog::error("Failed to create SDL window and renderer,SDL Error:{}.", SDL_GetError());
            return false;
        }

        if (!SDL_SetRenderLogicalPresentation(Renderer_, 640, 360, SDL_LOGICAL_PRESENTATION_LETTERBOX))
        {
            spdlog::error("Failed to set SDL logical presentation,SDL Error:{}.", SDL_GetError());
            return false;
        }
        
        spdlog::info("SDL initialization.");
        return true;
    }

    bool GameApp::InitializeTime()
    {
        try
        {
            Time_ = std::make_unique<Time>();
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to initialize time: {}", e.what());
            return false;
        }
        return true;
    }

    bool GameApp::InitializeResourceManager()
    {
        try
        {
            ResourceManager_ = std::make_unique<resource::ResourceManager>(Renderer_);
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to initialize resource manager: {}", e.what());
            return false;
        }
        return true;
    }
    void GameApp::HandleInput()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
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
        spdlog::info("Game closing.");
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
