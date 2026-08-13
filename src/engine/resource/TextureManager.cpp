#include "TextureManager.h"
#include <spdlog/spdlog.h>
#include <SDL3_image/SDL_image.h>

namespace engine::resource
{
    TextureManager::TextureManager(SDL_Renderer *renderer) : Renderer_(renderer)
    {
        if (!Renderer_)
        {
            throw std::runtime_error("TextureManager initialization failed,Error:Renderer is null.");
        }
        spdlog::info("TextureManager initialization.");
    }
    TextureManager::~TextureManager()
    {
        ClearTextures();
    }

    SDL_Texture *TextureManager::LoadTexture(const std::string &filePath)
    {
        // 检查纹理是否已经加载
        auto it = Textures_.find(filePath);
        if (it != Textures_.end())
        {
            return it->second.get();
        }

        // 加载纹理
        SDL_Texture *rawTexture = IMG_LoadTexture(Renderer_, filePath.c_str());
        if (!rawTexture)
        {
            spdlog::error("Failed to load texture from file: {}, Error: {}", filePath, SDL_GetError());
            return nullptr;
        }


        // 缓存纹理
        Textures_.emplace(filePath, std::unique_ptr<SDL_Texture, SDLTextureDeleter>(rawTexture));
        spdlog::info("Texture loaded and cached: {}", filePath);
        return rawTexture;
    }

    SDL_Texture *TextureManager::GetTexture(const std::string &filePath)
    {
        auto it = Textures_.find(filePath);
        if (it != Textures_.end())
        {
            return it->second.get();
        }
        else
        {
            spdlog::warn("Texture not found in cache: {},try loading it.", filePath);
            return LoadTexture(filePath);
        }
    }

    glm::vec2 TextureManager::GetTextureSize(const std::string &filePath)
    {
        SDL_Texture *texture = GetTexture(filePath);
        if (!texture)
        {
            spdlog::error("Failed to get texture size, texture not found: {}", filePath);
            return glm::ivec2(0, 0);
        }

        glm::vec2 size;
        if (!SDL_GetTextureSize(texture, &size.x, &size.y))
        {
            spdlog::error("Failed to get texture size for: {}, Error: {}", filePath, SDL_GetError());
            return glm::ivec2(0, 0);
        }
        return size;
    }

    void TextureManager::UnloadTexture(const std::string &filePath)
    {
        auto it = Textures_.find(filePath);
        if (it != Textures_.end())
        {
            Textures_.erase(it);
            spdlog::info("Texture unloaded and removed from cache: {}", filePath);
        }
        else
        {
            spdlog::warn("Attempted to unload texture not found in cache: {}", filePath);
        }
    }
    void TextureManager::ClearTextures()
    {
        Textures_.clear();
        spdlog::info("All textures cleared from cache.");
    }
    
} // namespace engine::resource
