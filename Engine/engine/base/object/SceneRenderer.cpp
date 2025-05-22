#include "SceneRenderer.h"

#include "ParticleManager.h"

void SceneRenderer::Finalize()
{
	objects_.clear();
	sprites_.clear();
	lines_.clear();
	primitiveDrawrs_.clear();
	animations_.clear();
}

void SceneRenderer::AllDraw()
{
	/// ============================== Object3d ============================== ///
	for (auto& object : objects_) {
		if (!object->GetRenderOptions().enabled || !object->GetRenderOptions().offscreen) { continue; }
		object->Draw();
	}
	/// ============================== Sprite ============================== ///
	for (auto& sprite : sprites_) {
		if (!sprite->GetRenderOptions().enabled || !sprite->GetRenderOptions().offscreen) { continue; }
		sprite->Draw();
	}
	/// ============================== Line3d ============================== ///
	for (auto& line : lines_) {
		if (!line->GetRenderOptions().enabled || !line->GetRenderOptions().offscreen) { continue; }
		if (line->GetIsMultiple()) { line->Draws(); }
		else { line->Draws(); }
	}
	/// ============================== Animation ============================== ///
	for (auto& animation : animations_) {
		if (!animation->GetRenderOptions().enabled || !animation->GetRenderOptions().offscreen) { continue; }
		animation->Draw();
	}
	/// ============================== PrimitiveDrawr ============================== ///
	for (auto& primitive : primitiveDrawrs_) {
		if (!primitive->GetRenderOptions().enabled || !primitive->GetRenderOptions().offscreen) { continue; }
		if (primitive->GetPrimitiveType() == PrimitiveType::None) { primitive->Draw(); }
		else { primitive->TypeDraw(); }
	}

	ParticleManager::GetInstance()->Draw();
}

void SceneRenderer::OutAllDraw()
{
	/// ============================== Object3d ============================== ///
	for (auto& object : objects_) {
		if (!object->GetRenderOptions().enabled || object->GetRenderOptions().offscreen) { continue; }
		object->Draw();
	}
	/// ============================== Sprite ============================== ///
	for (auto& sprite : sprites_) {
		if (!sprite->GetRenderOptions().enabled || sprite->GetRenderOptions().offscreen) { continue; }
		sprite->Draw();
	}
	/// ============================== Line3d ============================== ///
	for (auto& line : lines_) {
		if (!line->GetRenderOptions().enabled || line->GetRenderOptions().offscreen) { continue; }
		if (line->GetIsMultiple()) { line->Draws(); }
		else { line->Draws(); }
	}
	/// ============================== Animation ============================== ///
	for (auto& animation : animations_) {
		if (!animation->GetRenderOptions().enabled || animation->GetRenderOptions().offscreen) { continue; }
		animation->Draw();
	}
	/// ============================== PrimitiveDrawr ============================== ///
	for (auto& primitive : primitiveDrawrs_) {
		if (!primitive->GetRenderOptions().enabled || primitive->GetRenderOptions().offscreen) { continue; }
		if (primitive->GetPrimitiveType() == PrimitiveType::None) { primitive->Draw(); }
		else { primitive->TypeDraw(); }
	}
}
