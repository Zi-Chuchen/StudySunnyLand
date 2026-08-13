#include "AudioManager.h"
#include <spdlog/spdlog.h>

namespace engine::resource
{
    AudioManager::AudioManager()
    {
        spdlog::info("AudioManager initialization.");
        // 音频混音器初始化
        if (!MIX_Init())
        {
            spdlog::error("Failed to initialize SDL mixer library,SDL Error:{}.", SDL_GetError());
            throw std::runtime_error("Failed to initialize SDL mixer library.");
        }
        Mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
        if (!Mixer_)
        {
            spdlog::error("Failed to create mixer on default device,SDL Error:{}.", SDL_GetError());
            throw std::runtime_error("Failed to create mixer on default device.");
        }
    }
    AudioManager::~AudioManager()
    {
        MIX_StopAllTracks(Mixer_, 0);

        ClearAudios();

        if (Mixer_ != nullptr)
        {
            MIX_DestroyMixer(Mixer_);
            Mixer_ = nullptr;
        }
        MIX_Quit();
    }
    MIX_Track *AudioManager::LoadAudio(const std::string &filePath)
    {
        auto it = AudioCache_.find(filePath);
        if (it != AudioCache_.end())
        {
            return it->second.get();
        }

        MIX_Audio *audio = MIX_LoadAudio(Mixer_, filePath.c_str(), false);
        if (!audio)
        {
            spdlog::error("Failed to load audio file: {}, SDL Error: {}.", filePath, SDL_GetError());
            return nullptr;
        }
        MIX_Track *track = MIX_CreateTrack(Mixer_);
        if (!track)
        {
            spdlog::error("Failed to create track for audio file: {}, SDL Error: {}.", filePath, SDL_GetError());
            MIX_DestroyAudio(audio);
            return nullptr;
        }
        if (!MIX_SetTrackAudio(track, audio))
        {
            spdlog::error("Failed to set audio for track for audio file: {}, SDL Error: {}.", filePath, SDL_GetError());
            MIX_DestroyAudio(audio);
            MIX_DestroyTrack(track);
            return nullptr;
        }
        MIX_DestroyAudio(audio); // 释放音频数据，因为它已经被设置到轨道上了
        AudioCache_.emplace(filePath, std::unique_ptr<MIX_Track, SDLTrackDeleter>(track));
        spdlog::info("Audio loaded and cached: {}", filePath);
        return track;
    }
    MIX_Track *AudioManager::GetAudio(const std::string &filePath)
    {
        auto it = AudioCache_.find(filePath);
        if (it != AudioCache_.end())
        {
            return it->second.get();
        }

        spdlog::warn("Audio not found in cache: {}, try loading it.", filePath);
        return LoadAudio(filePath);
    }
    void AudioManager::UnloadAudio(const std::string &filePath)
    {
        auto it = AudioCache_.find(filePath);
        if (it != AudioCache_.end())
        {
            AudioCache_.erase(it);
            spdlog::info("Audio unloaded and removed from cache: {}", filePath);
        }
        else
        {
            spdlog::warn("Attempted to unload audio not in cache: {}", filePath);
        }
    }
    void AudioManager::ClearAudios()
    {
        AudioCache_.clear();
        spdlog::info("All audios cleared from cache.");
    }

} // namespace engine::resource
