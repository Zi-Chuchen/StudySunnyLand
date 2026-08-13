#include "Time.h"
#include <SDL3/SDL_timer.h>
#include <spdlog/spdlog.h>

namespace engine::core
{
    Time::Time()
    {
        // 初始化
        LastTime_ = NowTime_ = SDL_GetTicksNS();
        spdlog::info("Time initialization, the first time:{}.", LastTime_);
    }
    Time::~Time() = default;

    void Time::Update()
    {
        // 记录现在时间
        NowTime_ = SDL_GetTicksNS();

        auto deltaTime = static_cast<double>(NowTime_ - LastTime_) / 1e9;

        if (TargetDeltaTime_ > 0)
        {
            // 设置FPS则限制
            LimitFPS(deltaTime);
        }
        else
        {
            DeltaTime_ = deltaTime;
        }

        // 当前帧处理完毕,此时记录对下一次更新相当于上一帧时间
        LastTime_ = SDL_GetTicksNS();
    }

    void Time::LimitFPS(float realDeltaTime)
    {
        if (realDeltaTime < TargetDeltaTime_)
        {
            // 小于则延迟
            double needToWait = TargetDeltaTime_ - realDeltaTime;
            Uint64 needToWaitNS = static_cast<Uint64>(needToWait * 1e9);
            SDL_DelayNS(needToWaitNS);

            // 更新时间差
            DeltaTime_ = static_cast<double>(SDL_GetTicksNS() - LastTime_) / 1e9;
        }
        else
        {
            // 大于也需要更新
            DeltaTime_ = static_cast<double>(SDL_GetTicksNS() - LastTime_) / 1e9;
        }
    }

    float Time::GetDeltaTime() const
    {
        return DeltaTime_ * TimaeScale_;
    }

    float Time::GetUnscaledDeltaTime() const
    {
        return DeltaTime_;
    }

    void Time::SetTimeSacale(float timeScale)
    {
        TimaeScale_ = timeScale;
    }

    float Time::GetTimeScale() const
    {
        return TimaeScale_;
    }

    void Time::SetTargetFPS(int targetFPS)
    {
        if (targetFPS < 0)
        {
            spdlog::warn("The target frame rate is negative and has been automatically adjusted to 0 (unlimited).");
            TargetFPS_ = 0;
        }
        else
        {
            TargetFPS_ = targetFPS;
        }

        if (TargetFPS_ > 0)
        {
            TargetDeltaTime_ = 1.0 / static_cast<double>(TargetFPS_);
            spdlog::info("Target frame rate: {}, time difference: {:.6f}s.", TargetFPS_, TargetDeltaTime_);
        }
        else
        {
            TargetDeltaTime_ = 0;
            spdlog::info("Target frame rate: Unlimited");
        }
    }

    int Time::GetTargetFPS() const
    {
        return TargetFPS_;
    }
} // namespace engine::core
