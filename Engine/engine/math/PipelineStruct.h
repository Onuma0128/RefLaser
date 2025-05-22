#pragma once

/* PipelineのTypeを決める */ 
enum class PipelineType {
	Object3d,
	Sprite,
	Line3d,
	Particle,
	PrimitiveDrawr,
	Animation,
	RenderTexture,
	Skybox
};

enum class BlendMode {
	kBlendModeNone,			// ブレンド無し
	kBlendModeNormal,		// 通常αブレンド
	kBlendModeAdd,			// 加算ブレンド
	kBlendModeSubtract,		// 減算ブレンド
	kBlendModeMultily,		// 乗算ブレンド
	kCount
};