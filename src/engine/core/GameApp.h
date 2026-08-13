#pragma once
#include <memory>
// 前置声明
struct SDL_Window;
struct SDL_Renderer;

namespace engine::resource      
{
    class ResourceManager; // 资源管理器前置声明
}

namespace engine::core
{

    class Time;

    /**
     * @brief 创建游戏实例
     */
    class GameApp final
    {
    private:
        SDL_Window *Window_ = nullptr;
        SDL_Renderer *Renderer_ = nullptr;
        bool IsRunning_ = false;

        std::unique_ptr<Time> Time_;
        std::unique_ptr<resource::ResourceManager> ResourceManager_;

        void TestResourceManager();
    public:
        GameApp();
        ~GameApp();

        // 拷贝与移动都删除
        GameApp(const GameApp &) = delete;
        GameApp &operator=(const GameApp &) = delete;
        GameApp(const GameApp &&) = delete;
        GameApp &operator=(const GameApp &&) = delete;

        /**
         * @brief 运行游戏，自动管理内存，退出自动处理
         */
        void Run();

    private:
        [[nodiscard]] bool Initalize();
        [[nodiscard]] bool InitalizeSDL();
        [[nodiscard]] bool InitializeTime();
        [[nodiscard]] bool InitializeResourceManager();
        void HandleInput();
        void Update(float);
        void Render();
        void Close();
    };

} // namespace engine::core
