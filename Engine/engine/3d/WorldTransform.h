#pragma once

#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <wrl.h>

#include "Vector3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"

using Microsoft::WRL::ComPtr;

class Model;

class WorldTransform
{
public:

	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInverseTranspose;
	};
	
	WorldTransform();
	~WorldTransform() = default;

	// 行列更新
	void TransferMatrix(const Model* model);
	void TransferMatrix(const Matrix4x4& localMatrix);

	/*==================== アクセッサ ====================*/

	// 定数バッファの取得
	ID3D12Resource* GetConstBuffer()const { return constBuffer_.Get(); }

private:

	// 定数バッファ
	void CreateConstBuffer();

public:

	// ローカルスケール
	Vector3 scale_ = { 1, 1, 1 };
	// ローカル回転角
	Quaternion rotation_ = Quaternion::IdentityQuaternion();
	// ローカル座標
	Vector3 translation_ = { 0, 0, 0 };
	// ローカル → ワールド変換行列
	Matrix4x4 matWorld_ = Matrix4x4::Identity();
	// 親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;

private:

	/*==================== 座標変換行列 ====================*/

	// 定数バッファ
	ComPtr<ID3D12Resource> constBuffer_ = nullptr;
	// マッピングアドレス
	TransformationMatrix* constMap_ = nullptr;

};