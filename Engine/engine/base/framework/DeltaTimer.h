#pragma once

#include <Windows.h>

class DeltaTimer
{
public:

    static void Initialize();

    static void Update();

    static float GetDeltaTime() { return deltaTime_ * timeScale_; }

    static void  SetTimeScaleForSeconds(float scale, float duration);

private:
    static inline LARGE_INTEGER frequency_;
    static inline LARGE_INTEGER startTime_;
    static inline LARGE_INTEGER prevTime_;
    static inline float deltaTime_ = 0.0f;      // 今の時間
    static inline float  timeScale_ = 1.0f;     // 時間のスケール
    static inline float  scaleTimer_ = 0.0f;    // 時間のスケールをしている秒数
};
