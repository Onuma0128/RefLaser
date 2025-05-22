#include "PlayerBulletUI.h"

void PlayerBulletUI::Init(const Vector2& position)
{
	Sprite::Initialize("uvChecker.png");
	Sprite::SetSceneRenderer();
	
	transform_.size = { 32.0f,32.0f };
	transform_.position = position;
}

void PlayerBulletUI::Update(const Vector2& position)
{
	//transform_.position = position;

	Sprite::Update();
}

