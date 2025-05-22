#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <wrl.h>
#include <memory>
#include <stdint.h>
#include <string>

#include "SpriteBase.h"

#include "ModelStruct.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Transform.h"

using Microsoft::WRL::ComPtr;

class Sprite
{
public:

	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct Material {
		Vector4 color;
		Matrix4x4 uvTransform;
	};

	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInverseTranspose;
	};

	/*==================== メンバ関数 ====================*/

	void Initialize(std::string textureFilePath);
	void SetSceneRenderer();

	void Update();

	void Draw();

	/*==================== アクセッサ ====================*/

	// トランスフォーム
	Transform2D& GetTransform() { return transform_; }

	// カラー
	const Vector4& GetColor()const { return materialData_->color; }
	void SetColor(const Vector4& color) { materialData_->color = color; }

	// アンカーポイント
	const Vector2& GetAnchorPoint()const { return anchorPoint_; }
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint_ = anchorPoint; }

	// 左右フリップ
	const bool& GetFlipX()const { return isFlipX_; }
	void SetFlipX(const bool& isFlipX) { this->isFlipX_ = isFlipX; }
	// 上下フリップ
	const bool& GetFlipY()const { return isFlipY_; }
	void SetFlipY(const bool& isFlipY) { this->isFlipY_ = isFlipY; }

	// テクスチャ左上座標
	const Vector2& GetTextureLeftTop()const { return textureLeftTop_; }
	void SetTextureLeftTop(const Vector2& leftTop) { textureLeftTop_ = leftTop; }
	// テクスチャ切り出しサイズ
	const Vector2& GetTextureSize()const { return textureSize_; }
	void SetTextureSize(const Vector2& size) { textureSize_ = size; }

	RenderOptions& GetRenderOptions() { return renderOptions_; }

private:

	void VertexDataInitialize();

	void MaterialDataInitialize();

	void TransformationMatrixDataInitialize();

	void AccessorUpdate();

	void UpdateMatrix();

	void AdjustTextureSize();

private:

	std::unique_ptr<SpriteBase> spriteBase_ = nullptr;
	// テクスチャ番号　
	uint32_t textureIndex_ = 0;
	// ファイルパス
	std::string textureFilePath_;

	// バッファリソース
	ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	ComPtr<ID3D12Resource> indexResource_ = nullptr;
	// バッファリソース内のデータ
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	// バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	// マテリアルリソース
	ComPtr<ID3D12Resource> materialResource_ = nullptr;
	// マテリアルデータ
	Material* materialData_ = nullptr;

	// 座標変換行列
	ComPtr<ID3D12Resource> transformationMatrixResource_ = nullptr;
	TransformationMatrix* transformationMatrixData_ = nullptr;

	// 描画するか
	RenderOptions renderOptions_;

protected:


	/*==================== アクセッサー用のメンバ変数 ====================*/
	
	// トランスフォーム
	Transform2D transform_{ {1.0f,1.0f},0.0f,{0.0f,0.0f} };
	// アンカーポイント
	Vector2 anchorPoint_ = { 0.0f,0.0f };
	// 左右フリップ
	bool isFlipX_ = false;
	// 上下フリップ
	bool isFlipY_ = false;
	// テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	// テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 64.0f,64.0f };
};

