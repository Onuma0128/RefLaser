#include "Wall.h"

void Wall::Init()
{
	Object3d::Initialize("Box.obj");
	Object3d::SetSceneRenderer();

	Collider::AddCollider();
	Collider::colliderName_ = "Wall";
	Collider::myType_ = ColliderType::OBB;
	Collider::size_ = transform_.scale_;
	Collider::DrawCollider();
}

void Wall::Update()
{



	Collider::rotate_ = transform_.rotation_;
	Collider::centerPosition_ = transform_.translation_;
	Collider::Update();
	Object3d::Update();
}

void Wall::OnCollisionEnter(Collider* other)
{
}

void Wall::OnCollisionStay(Collider* other)
{
}

void Wall::OnCollisionExit(Collider* other)
{
}
