#pragma once
#include <cmath>
#include <numbers>
#include <algorithm>

class Easing {
public:
    static float EaseInSine(float t) {
        return 1.0f - std::cos((t * std::numbers::pi_v<float>) / 2.0f);
    }

    static float EaseOutSine(float t) {
        return std::sin((t * std::numbers::pi_v<float>) / 2.0f);
    }

    static float EaseInOutSine(float t) {
        return -(std::cos(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
    }

    static float EaseInQuad(float t) {
        return t * t;
    }

    static float EaseOutQuad(float t) {
        return 1.0f - (1.0f - t) * (1.0f - t);
    }

    static float EaseInOutQuad(float t) {
        return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
    }

    static float EaseInCubic(float t) {
        return t * t * t;
    }

    static float EaseOutCubic(float t) {
        return 1.0f - std::pow(1.0f - t, 3.0f);
    }

    static float EaseInOutCubic(float t) {
        return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
    }

    static float EaseInQuart(float t) {
        return t * t * t * t;
    }

    static float EaseOutQuart(float t) {
        return 1.0f - std::pow(1.0f - t, 4.0f);
    }

    static float EaseInOutQuart(float t) {
        return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
    }

    static float EaseInQuint(float t) {
        return t * t * t * t * t;
    }

    static float EaseOutQuint(float t) {
        return 1.0f - std::pow(1.0f - t, 5.0f);
    }

    static float EaseInOutQuint(float t) {
        return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 5.0f) / 2.0f;
    }

    static float EaseInExpo(float t) {
        return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f);
    }

    static float EaseOutExpo(float t) {
        return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
    }

    static float EaseInOutExpo(float t) {
        if (t == 0.0f) return 0.0f;
        if (t == 1.0f) return 1.0f;
        return t < 0.5f
            ? std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f
            : (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
    }

    static float EaseInCirc(float t) {
        return 1.0f - std::sqrt(1.0f - t * t);
    }

    static float EaseOutCirc(float t) {
        return std::sqrt(1.0f - std::pow(t - 1.0f, 2.0f));
    }

    static float EaseInOutCirc(float t) {
        return t < 0.5f
            ? (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f
            : (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
    }

    static float EaseInBack(float t) {
        float c1 = 1.70158f;
        float c3 = c1 + 1.0f;
        return c3 * t * t * t - c1 * t * t;
    }

    static float EaseOutBack(float t) {
        float c1 = 1.70158f;
        float c3 = c1 + 1.0f;
        return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
    }

    static float EaseInOutBack(float t) {
        float c1 = 1.70158f;
        float c2 = c1 * 1.525f;

        return t < 0.5f
            ? (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
            : (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
    }
};
