#include "Line3d.h"

#include <numbers>

#include "DirectXEngine.h"

#include "Camera.h"
#include "CameraManager.h"
#include "CreateBufferResource.h"

void Line3d::Initialize(const std::vector<Vector3>& positions)
{
	assert((positions.size() & 1u) == 0 && "偶数個の座標が必要です");
	lineCount_ = static_cast<uint32_t>(positions.size() / 2);

	line3dBase_ = std::make_unique<Line3dBase>();
	line3dBase_->Initialize();

	CreateLocalVB();
	CreateInstanceVB(positions);
	CreateCB();

	renderOptions_ = {
		.enabled = true,
		.offscreen = true
	};
	isMultiple_ = true;
	DirectXEngine::GetSceneRenderer()->SetDrawList(this);
}

void Line3d::Update()
{
	*wvpData_ = CameraManager::GetInstance()->GetActiveCamera()->GetViewProjectionMatrix();
}

void Line3d::Draws()
{
	line3dBase_->DrawBase();

	auto commandList = DirectXEngine::GetCommandList();
	D3D12_VERTEX_BUFFER_VIEW views[2] = { vbvLocal_, vbvInst_ };
	commandList->IASetVertexBuffers(0, 2, views);
	commandList->SetGraphicsRootConstantBufferView(0, wvpResource_->GetGPUVirtualAddress());
	commandList->DrawInstanced(2, lineCount_, 0, 0);
}

void Line3d::CreateLocalVB()
{
	LocalVertex local[2] = { {0.f}, {1.f} };   // t=0 / t=1
	auto device = DirectXEngine::GetDevice();
	localVB_ = CreateBufferResource(device, sizeof(local)).Get();
	void* dst{};
	localVB_->Map(0, nullptr, &dst);
	memcpy(dst, local, sizeof(local));
	localVB_->Unmap(0, nullptr);

	vbvLocal_.BufferLocation = localVB_->GetGPUVirtualAddress();
	vbvLocal_.SizeInBytes = sizeof(local);
	vbvLocal_.StrideInBytes = sizeof(LocalVertex);
}

void Line3d::CreateInstanceVB(const std::vector<Vector3>& pos)
{
	std::vector<LineInstance> inst(lineCount_);
	for (uint32_t i = 0; i < lineCount_; ++i)
	{
		inst[i].startPos = pos[i * 2];
		inst[i].endPos = pos[i * 2 + 1];
	}

	auto bytes = inst.size() * sizeof(LineInstance);
	auto device = DirectXEngine::GetDevice();
	instanceVB_ = CreateBufferResource(device, bytes).Get();

	void* dst{};
	instanceVB_->Map(0, nullptr, &dst);
	memcpy(dst, inst.data(), bytes);
	instanceVB_->Unmap(0, nullptr);

	vbvInst_.BufferLocation = instanceVB_->GetGPUVirtualAddress();
	vbvInst_.SizeInBytes = bytes;
	vbvInst_.StrideInBytes = sizeof(LineInstance);
}

// ──────────────────────────────────────────────────────────────
void Line3d::CreateCB()
{
	auto device = DirectXEngine::GetDevice();
	wvpResource_ = CreateBufferResource(device, sizeof(Matrix4x4)).Get();
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
}

void Line3d::SetPositions(const std::vector<Vector3>& positions)
{
	std::vector<LineInstance> inst(lineCount_);
	for (uint32_t i = 0; i < lineCount_; ++i)
	{
		inst[i].startPos = positions[i * 2];
		inst[i].endPos = positions[i * 2 + 1];
	}

	auto bytes = inst.size() * sizeof(LineInstance);
	void* dst{};
	instanceVB_->Map(0, nullptr, &dst);
	memcpy(dst, inst.data(), bytes);
}

std::vector<Vector3> Line3d::CreateBox(const Vector3& min, const Vector3& max)
{
	std::vector<Vector3> linePosition = {
		min,
		{ max.x, min.y, min.z },

		min,
		{ min.x, max.y, min.z },

		min,
		{ min.x, min.y, max.z },

		{ min.x, max.y, min.z },
		{ max.x, max.y, min.z },

		{ min.x, max.y, min.z },
		{ min.x, max.y, max.z },

		{ min.x, min.y, max.z },
		{ max.x, min.y, max.z },

		{ max.x, min.y, min.z },
		{ max.x, min.y, max.z },

		{ min.x, min.y, max.z },
		{ min.x, max.y, max.z },

		{ max.x, min.y, min.z },
		{ max.x, max.y, min.z },

		{ min.x, max.y, max.z },
		max,

		{ max.x, min.y, max.z },
		max,

		{ max.x, max.y, min.z },
		max,
	};

	return linePosition;
}

std::vector<Vector3> Line3d::CreateSphere(const float& radius)
{
	const int latitudeLines = 6;   // 緯線の分割数
	const int longitudeLines = 12;  // 経線の分割数

	std::vector<Vector3> linePosition;

	// 緯線（Z軸まわりのX-Y平面）
	for (int i = 0; i < latitudeLines; ++i) {
		float theta1 = (float)i / latitudeLines * std::numbers::pi_v<float>;
		float theta2 = (float)(i + 1) / latitudeLines * std::numbers::pi_v<float>;

		for (int j = 0; j < longitudeLines; ++j) {
			float phi = (float)j / longitudeLines * (std::numbers::pi_v<float> * 2.0f);

			Vector3 p1 = {
				radius * std::sin(theta1) * std::cos(phi),
				radius * std::cos(theta1),
				radius * std::sin(theta1) * std::sin(phi)
			};

			Vector3 p2 = {
				radius * std::sin(theta2) * std::cos(phi),
				radius * std::cos(theta2),
				radius * std::sin(theta2) * std::sin(phi)
			};

			linePosition.push_back(p1);
			linePosition.push_back(p2);
		}
	}

	// 経線（X軸またはY軸に固定した断面）
	for (int i = 0; i < longitudeLines; ++i) {
		float phi1 = (float)i / longitudeLines * (std::numbers::pi_v<float> * 2.0f);
		float phi2 = (float)(i + 1) / longitudeLines * (std::numbers::pi_v<float> *2.0f);

		for (int j = 0; j < latitudeLines; ++j) {
			float theta = (float)j / latitudeLines * std::numbers::pi_v<float>;

			Vector3 p1 = {
				radius * std::sin(theta) * std::cos(phi1),
				radius * std::cos(theta),
				radius * std::sin(theta) * std::sin(phi1)
			};

			Vector3 p2 = {
				radius * std::sin(theta) * std::cos(phi2),
				radius * std::cos(theta),
				radius * std::sin(theta) * std::sin(phi2)
			};

			linePosition.push_back(p1);
			linePosition.push_back(p2);
		}
	}

	return linePosition;
}
