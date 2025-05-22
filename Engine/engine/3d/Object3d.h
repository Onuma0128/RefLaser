#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <wrl.h>
#include <memory>
#include <string>

#include "Object3dBase.h"
#include "WorldTransform.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#include "ModelStruct.h"

using Microsoft::WRL::ComPtr;

class Model;

class Object3d
{
public:
	/*==================== メンバ関数 ====================*/

	// 初期化
	void Initialize(const std::string& filePath);
	void SetSceneRenderer();

	// 更新
	void Update();

	// 描画
	void Draw();

	/*==================== アクセッサ ====================*/

	void SetModel(const std::string& filePath);
	void SetTexture(const std::string& directoryPath, const std::string& filePath);
	void SetTexture_ENV(const std::string& directoryPath, const std::string& filePath);
	void SetColor(const Vector4& color);

	WorldTransform& GetTransform() { return transform_; }
	Model* GetModel() { return model_; }
	Material* GetMaterial() { return materialData_; }
	RenderOptions& GetRenderOptions() { return renderOptions_; }

private:

	// マテリアルデータの作成
	void MakeMaterialData();

protected:

	WorldTransform transform_;

private:

	std::unique_ptr<Object3dBase> object3dBase_ = nullptr;

	Model* model_ = nullptr;

	/*==================== マテリアル ====================*/

	// バッファリソース
	ComPtr<ID3D12Resource> materialResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	Material* materialData_ = nullptr;

	// 描画するか
	RenderOptions renderOptions_;

};