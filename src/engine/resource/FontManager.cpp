#include "FontManager.h"
#include <spdlog/spdlog.h>
namespace engine::resource
{
    FontManager::FontManager()
    {
        spdlog::info("FontManager initialization.");
        if (!TTF_WasInit() && !TTF_Init())
        {
            spdlog::error("Failed to initialize SDL ttf library,SDL Error:{}.", SDL_GetError());
            throw std::runtime_error("Failed to initialize SDL ttf library.");
        }
    }

    FontManager::~FontManager()
    {
        ClearFonts();
        if (TTF_WasInit())
        {
            TTF_Quit();
        }
    }
    TTF_Font* FontManager::LoadFont(const std::string &filePath, int fontSize)
    {
        if (fontSize <= 0)
        {
            spdlog::error("Invalid font size: {}. Font size must be greater than 0.", fontSize);
            return nullptr;
        }

        FontKey key(filePath, fontSize);
        auto it = FontCache_.find(key);
        if (it != FontCache_.end())
        {
            return it->second.get();
        }

        TTF_Font *font = TTF_OpenFont(filePath.c_str(), fontSize);
        if (!font)
        {
            spdlog::error("Failed to load font file: {}, SDL Error: {}.", filePath, SDL_GetError());
            return nullptr;
        }
        FontCache_.emplace(key, std::unique_ptr<TTF_Font, SDLFontDeleter>(font));
        return font;
    }
    TTF_Font* FontManager::GetFont(const std::string &filePath, int fontSize)
    {
        FontKey key(filePath, fontSize);
        auto it = FontCache_.find(key);
        if (it != FontCache_.end())
        {
            return it->second.get();
        }

        spdlog::warn("Font not found in cache: {} with size {}. Attempting to load.", filePath, fontSize);
        return LoadFont(filePath, fontSize);
    }

    void FontManager::UnloadFont(const std::string &filePath, int fontSize)
    {
        FontKey key(filePath, fontSize);
        auto it = FontCache_.find(key);
        if (it != FontCache_.end())
        {
            FontCache_.erase(it);
            spdlog::info("Font unloaded: {} with size {}.", filePath, fontSize);
        }
        else
        {
            spdlog::warn("Attempted to unload font not in cache: {} with size {}.", filePath, fontSize);
        }
    }

    void FontManager::ClearFonts()
    {
        FontCache_.clear();
        spdlog::info("All fonts cleared from cache.");
    }
} // namespace engine::resource
