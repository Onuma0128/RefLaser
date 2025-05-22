#pragma once

#include "assimp//Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "ModelStruct.h"
#include "AnimationStruct.h"

class ModelBase;

class Model
{
public:
	/*==================== メンバ関数 ====================*/

		// 初期化
	void Initialize(const std::string& directoryPath, const std::string& filename);

	// 描画
	void Draw(bool isAnimation);

	const ModelData GetModelData()const { return modelData_; }
	const D3D12_VERTEX_BUFFER_VIEW GetVertexBuffer() { return vertexBufferView_; }

	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	void SetTexture(const std::string& directoryPath, const std::string& filename);
	void SetTexture_ENV(const std::string& directoryPath, const std::string& filename);

private:

	// 頂点データの作成
	void MakeVertexData();
	void MakeIndexData();

	static std::wstring s2ws(const std::string& str);

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	static Node ReadNode(aiNode* node);


private:

	ModelBase* modelBase_ = nullptr;

	/*==================== モデルのデータ ====================*/

	ModelData modelData_;

	/*==================== 頂点データ ====================*/

	// バッファリソース
	ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	/*==================== Indexデータ ====================*/

	// バッファリソース
	ComPtr<ID3D12Resource> indexResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	uint32_t* indexData_ = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

};