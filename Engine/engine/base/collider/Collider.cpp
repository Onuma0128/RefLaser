#include "Collider.h"

#include "DirectXEngine.h"

#include "Line3d.h"

Collider::~Collider()
{
	DirectXEngine::GetCollisionMgr()->RemoveCollider(this);
}

void Collider::AddCollider()
{
	DirectXEngine::GetCollisionMgr()->AddCollider(this);
}

void Collider::DrawCollider()
{
#ifdef _DEBUG
	// Lineの初期化
	line_ = std::make_unique<Line3d>();
	linePositions_ = CreateLinePositions();
	line_->Initialize(linePositions_);
#endif // _DEBUG	
}

void Collider::Update()
{
	if (line_ == nullptr) { return; }

	linePositions_ = CreateLinePositions();
	for (auto& linePos : linePositions_) {
		Vector3 translate = centerPosition_;
		Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(rotate_);
		linePos = linePos.Transform(rotateMatrix) + translate;
	}
	line_->SetPositions(linePositions_);
	line_->Update();
}

std::vector<Vector3> Collider::CreateLinePositions()
{
	// Line用の座標を作成
	std::vector<Vector3> linePositions;
	switch (myType_)
	{
	case ColliderType::Sphere:
		linePositions = line_->CreateSphere(radius_);
		break;
	case ColliderType::Segment:
		break;
	case ColliderType::OBB:
		linePositions = line_->CreateBox(-size_, size_);
		break;
	default:
		break;
	}

	return linePositions;
}
