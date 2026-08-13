#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>

// 前置声明
struct SDL_Renderer;
struct SDL_Texture;
struct MIX_Track;
struct TTF_TextEngine;
struct TTF_Font;

namespace engine::resource
{
    // 资源管理器前置声明
    class TextureManager;
    class AudioManager;
    class FontManager;

    /**
     * @brief 资源管理器，负责加载和管理游戏资源，本身不负责加载，只是其他加载器的集合
     */
    class ResourceManager final{
        private:
        std::unique_ptr<TextureManager> TextureManager_;
        std::unique_ptr<AudioManager> AudioManager_;
        std::unique_ptr<FontManager> FontManager_;

        public:
        explicit ResourceManager(SDL_Renderer *renderer);
        ~ResourceManager();
        
        // 拷贝与移动都删除
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;
        
        // 获取与加载资源函数
        // 获取函数首先尝试从缓存中获取资源，如果没有则调用加载函数加载资源并缓存
        // Texture
        SDL_Texture* LoadTexture(const std::string& filePath);
        SDL_Texture* GetTexture(const std::string& filePath);
        glm::vec2 GetTextureSize(const std::string& filePath);
        void UnloadTexture(const std::string& filePath);
        void ClearTextures();

        // Audio
        MIX_Track* LoadAudio(const std::string& filePath);
        MIX_Track* GetAudio(const std::string& filePath);
        void UnloadAudio(const std::string& filePath);
        void ClearAudios();

        // Font
        TTF_Font* LoadFont(const std::string& filePath, int fontSize);
        TTF_Font* GetFont(const std::string& filePath, int fontSize);
        void UnloadFont(const std::string& filePath, int fontSize);
        void ClearFonts();
    };
} // namespace engine::resource

