#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <SDL3_mixer/SDL_mixer.h>

namespace engine::resource
{
    /**
     * @brief 音频管理器，负责加载和管理游戏音频资源
     */
    class AudioManager final
    {
        friend class ResourceManager; // 资源管理器是音频管理器的友元类，可以访问私有成员
    private:
        struct SDLTrackDeleter
        {
            void operator()(MIX_Track *track) const
            {
                if (track)
                {
                    MIX_StopTrack(track, 0);
                    MIX_DestroyTrack(track);
                }
            }
        };
        MIX_Mixer *Mixer_ = nullptr; // 音频混音器指针，用于创建音频
        std::unordered_map<std::string, std::unique_ptr<MIX_Track, SDLTrackDeleter>> AudioCache_;

    public:
        AudioManager();
        ~AudioManager();
        // 拷贝与移动都删除
        AudioManager(const AudioManager &) = delete;
        AudioManager &operator=(const AudioManager &) = delete;
        AudioManager(AudioManager &&) = delete;
        AudioManager &operator=(AudioManager &&) = delete;

        private:
        // 加载和获取音频函数
        MIX_Track *LoadAudio(const std::string &filePath);
        MIX_Track *GetAudio(const std::string &filePath);
        void UnloadAudio(const std::string &filePath);
        void ClearAudios();
    };
} // namespace engine::resource
