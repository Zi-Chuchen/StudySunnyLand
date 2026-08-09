#pragma once
#include <SDL3/SDL_stdinc.h>

namespace engine::core
{
    /**
     * @brief 管理游戏时间，提供时间缩放功能
     */
    class Time final
    {
    private:
        Uint64 LastTime_ = 0;  // 上一帧时间(ns)
        Uint64 NowTime_ = 0;   // 这一帧时间(ns)
        double DeltaTime_ = 0; // 时间差(s)
        double TimaeScale_ = 0; // 时间缩放因子

        // FPS
        int TargetFPS_ = 0;          // 目标FPS(0代表不限制)
        double TargetDeltaTime_ = 0; // 目标时间差
    public:
        Time();
        ~Time();

        // 拷贝与移动都删除
        Time(const Time &) = delete;
        Time &operator=(const Time &) = delete;
        Time(const Time &&) = delete;
        Time &operator=(const Time &&) = delete;

        /**
         * @brief 每帧开始时调用用于记录时间差
         */
        void Update();

        /**
         * @brief 获取缩放后的时间差
         *
         * @return float 缩放后的时间差
         */
        float GetDeltaTime() const;

        /**
         * @brief 获取未缩放的时间差
         *
         * @return float 未缩放的时间差
         */
        float GetUnscaledDeltaTime() const;

        /**
         * @brief 设置时间缩放因子
         */
        void SetTimeSacale(float);

        /**
         * @brief 获取时间缩放因子
         *
         * @return float 时间缩放因子
         */
        float GetTimeScale() const;

        /**
         * @brief 设置目标FPS
         */
        void SetTargetFPS(int FPS);

        /**
         * @brief 获取目标FPS
         *
         * @return int 目标FPS
         */
        int GetTargetFPS() const;

    private:
        /**
         * @brief 限制帧率
         * 
         * @param float 真实时间差
         */
        void LimitFPS(float);
    };

} // namespace engine::core
