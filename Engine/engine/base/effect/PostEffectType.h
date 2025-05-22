#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

enum class PostEffectType {
    None,
    RenderTexture,
    Grayscale,
    Vignette,
    Smoothing,
    OutLine,
    Count
};

struct GrayscaleData
{
    Vector3 color;
    float t;
};

struct VignetteData
{
    float scale;
    float gamma;
    float padding[2];
};

struct OutlineData
{
    Matrix4x4 projection;
};