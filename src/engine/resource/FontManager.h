#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include <utility>

namespace engine::resource
{
    using FontKey = std::pair<std::string, int>; // 字体文件路径和字体大小的组合键

    struct FontKeyHash
    {
        std::size_t operator()(const FontKey &key) const
        {
            return std::hash<std::string>()(key.first) ^ std::hash<int>()(key.second);
        }
    };

    class FontManager final
    {
        friend class ResourceManager; // 资源管理器是字体管理器的友元类，可以访问私有成员
    private:
        struct SDLFontDeleter
        {
            void operator()(TTF_Font *font) const
            {
                if (font)
                {
                    TTF_CloseFont(font);
                }
            }
        };

        std::unordered_map<FontKey, std::unique_ptr<TTF_Font, SDLFontDeleter>, FontKeyHash> FontCache_;

    public:
        FontManager();
        ~FontManager();

        // 拷贝与移动都删除
        FontManager(const FontManager &) = delete;
        FontManager &operator=(const FontManager &) = delete;
        FontManager(FontManager &&) = delete;
        FontManager &operator=(FontManager &&) = delete;

        private:
        // 加载和获取字体函数
        TTF_Font *LoadFont(const std::string &filePath, int fontSize);
        TTF_Font *GetFont(const std::string &filePath, int fontSize);
        void UnloadFont(const std::string &filePath, int fontSize);
        void ClearFonts();
    };
} // namespace engine::resource
