#include "Wall.h"

void Wall::Finalize()
{
	Object3d::SetRemove();
	Collider::RemoveCollider();
}

void Wall::Init()
{
	GlobalInit();
	Object3d::Initialize("Box.obj");
	Object3d::SetSceneRenderer();

	Vector3 scale = global_->GetValue<Vector3>("WallFactory", groupName_ + "/scale");
	scale.x = std::clamp(scale.x, 0.01f, 100.0f);
	scale.y = std::clamp(scale.y, 0.01f, 100.0f);
	scale.z = std::clamp(scale.z, 0.01f, 100.0f);
	transform_.scale_ = { scale.x,scale.y,scale.z };
	float angleY = global_->GetValue<float>("WallFactory", groupName_ + "/rotateY");
	transform_.rotation_ = Quaternion::MakeRotateAxisAngleQuaternion(Vector3::ExprUnitY, angleY);
	transform_.translation_ = global_->GetValue<Vector3>("WallFactory", groupName_ + "/position");

	Collider::AddCollider();
	Collider::colliderName_ = "Wall";
	Collider::myType_ = ColliderType::OBB;
	Collider::size_ = transform_.scale_;
	Collider::DrawCollider();
}

void Wall::Update()
{
#ifdef _DEBUG
	isDelete_ = global_->GetValue<bool>("WallFactory", groupName_ + "/Delete");
	DebugTransform();
#endif // _DEBUG

	Collider::size_ = transform_.scale_;
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

void Wall::GlobalInit()
{
	// Transform
	global_->AddValue<Vector3>("WallFactory", groupName_ + "/scale", { 1.0f,1.0f,1.0f });
	global_->AddValue<float>("WallFactory", groupName_ + "/rotateY", 0.0f);
	global_->AddValue<Vector3>("WallFactory", groupName_ + "/position", { 0.0f,0.0f,0.0f });
	// 削除するか
	global_->AddValue<bool>("WallFactory", groupName_ + "/Delete", false);
}

void Wall::DebugTransform()
{
	Vector3 scale = global_->GetValue<Vector3>("WallFactory", groupName_ + "/scale");
	scale.x = std::clamp(scale.x, 0.01f, 100.0f);
	scale.y = std::clamp(scale.y, 0.01f, 100.0f);
	scale.z = std::clamp(scale.z, 0.01f, 100.0f);
	transform_.scale_ = { scale.x,scale.y,scale.z };
	float angleY = global_->GetValue<float>("WallFactory", groupName_ + "/rotateY");
	transform_.rotation_ = Quaternion::MakeRotateAxisAngleQuaternion(Vector3::ExprUnitY, angleY);
	transform_.translation_ = global_->GetValue<Vector3>("WallFactory", groupName_ + "/position");
}
