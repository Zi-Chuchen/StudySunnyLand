#include "RescourceManager.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "FontManager.h"

namespace engine::resource
{
    ResourceManager::ResourceManager(SDL_Renderer *renderer)
    {
        TextureManager_ = std::make_unique<TextureManager>(renderer);
        AudioManager_ = std::make_unique<AudioManager>();
        FontManager_ = std::make_unique<FontManager>();
    }

    ResourceManager::~ResourceManager()
    {
        ClearTextures();
        ClearAudios();
        ClearFonts();
    }
    SDL_Texture *ResourceManager::LoadTexture(const std::string &filePath)
    {
        return TextureManager_->LoadTexture(filePath);
    }

    SDL_Texture *ResourceManager::GetTexture(const std::string &filePath)
    {
        return TextureManager_->GetTexture(filePath);
    }

    glm::vec2 ResourceManager::GetTextureSize(const std::string &filePath)
    {
        return TextureManager_->GetTextureSize(filePath);
    }

    void ResourceManager::UnloadTexture(const std::string &filePath)
    {
        TextureManager_->UnloadTexture(filePath);
    }

    void ResourceManager::ClearTextures()
    {
        TextureManager_->ClearTextures();
    }

    MIX_Track *ResourceManager::LoadAudio(const std::string &filePath)
    {
        return AudioManager_->LoadAudio(filePath);
    }

    MIX_Track *ResourceManager::GetAudio(const std::string &filePath)
    {
        return AudioManager_->GetAudio(filePath);
    }

    void ResourceManager::UnloadAudio(const std::string &filePath)
    {
        AudioManager_->UnloadAudio(filePath);
    }

    void ResourceManager::ClearAudios()
    {
        AudioManager_->ClearAudios();
    }

    TTF_Font *ResourceManager::LoadFont(const std::string &filePath, int fontSize)
    {
        return FontManager_->LoadFont(filePath, fontSize);
    }
    TTF_Font *ResourceManager::GetFont(const std::string &filePath, int fontSize)
    {
        return FontManager_->GetFont(filePath, fontSize);
    }

    void ResourceManager::UnloadFont(const std::string &filePath, int fontSize)
    {
        FontManager_->UnloadFont(filePath, fontSize);
    }

    void ResourceManager::ClearFonts()
    {
        FontManager_->ClearFonts();
    }

} // namespace engine::resource
