#pragma once
#include <list>
#include <memory>

#include "Object3d.h"
#include "Sprite.h"
#include "Line3d.h"
#include "PrimitiveDrawr.h"
#include "Animation.h"

class SceneRenderer
{
public:

	void Finalize();

	// 全ての描画
	void AllDraw();
	// 全ての描画(offscreenが終わってから描画)
	void OutAllDraw();

	// Obejctをセットする
	template<typename T>
	void SetDrawList(T* object);

private:

	/// ============================== 描画するObjects ============================== ///

	std::list<Object3d*>		objects_;
	std::list<Sprite*>			sprites_;
	std::list<Line3d*>			lines_;
	std::list<PrimitiveDrawr*>	primitiveDrawrs_;
	std::list<Animation*>		animations_;

};

template<>
inline void SceneRenderer::SetDrawList<Object3d>(Object3d* object) {
	objects_.push_back(object);
}

template<>
inline void SceneRenderer::SetDrawList<Sprite>(Sprite* sprite) {
	sprites_.push_back(sprite);
}

template<>
inline void SceneRenderer::SetDrawList<Line3d>(Line3d* line) {
	lines_.push_back(line);
}

template<>
inline void SceneRenderer::SetDrawList<PrimitiveDrawr>(PrimitiveDrawr* primitive) {
	primitiveDrawrs_.push_back(primitive);
}

template<>
inline void SceneRenderer::SetDrawList<Animation>(Animation* animation) {
	animations_.push_back(animation);
}
