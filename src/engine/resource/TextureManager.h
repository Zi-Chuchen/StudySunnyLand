#pragma once
#include <memory>
#include <string>
#include <SDL3/SDL_render.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include <unordered_map>

namespace engine::resource
{
    /**
     * @brief 纹理管理器，负责加载和管理游戏纹理资源
     */
    class TextureManager final
    {
        friend class ResourceManager; // 资源管理器是纹理管理器的友元类，可以访问私有成员
    private:
        struct SDLTextureDeleter
        {
            void operator()(SDL_Texture *texture) const
            {
                if (texture)
                {
                    SDL_DestroyTexture(texture);
                }
            }
        };

        std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDLTextureDeleter>> Textures_;

        SDL_Renderer *Renderer_ = nullptr; // 渲染器指针，用于创建纹理

    public:
        explicit TextureManager(SDL_Renderer *renderer);
        ~TextureManager();
        
        // 拷贝与移动都删除
        TextureManager(const TextureManager &) = delete;
        TextureManager &operator=(const TextureManager &) = delete;
        TextureManager(TextureManager &&) = delete;
        TextureManager &operator=(TextureManager &&) = delete;

    private:
        // 加载和获取纹理函数
        SDL_Texture *LoadTexture(const std::string &filePath);
        SDL_Texture *GetTexture(const std::string &filePath);
        void UnloadTexture(const std::string &filePath);
        glm::vec2 GetTextureSize(const std::string &filePath);
        void ClearTextures();
    };
} // namespace engine::resource
