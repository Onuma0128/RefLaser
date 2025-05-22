#pragma once
#include <memory>

#include "AnimationBase.h"
#include "WorldTransform.h"

#include "AnimationStruct.h"
#include "ModelStruct.h"

class Model;
class Line3d;

class Animation
{
public:

	void Init(const std::string& directoryPath, const std::string& filename);
	void SetSceneRenderer();

	void Update();

	void Draw();

	AnimationData LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	/*==================== アクセッサ ====================*/

	void SetModel(const std::string& filePath);
	void SetTexture(const std::string& directoryPath, const std::string& filePath);
	void SetColor(const Vector4& color);

	WorldTransform& GetTransform() { return transform_; }
	RenderOptions& GetRenderOptions() { return renderOptions_; }
	Material* GetMaterial() { return materialData_; }

private:

	Skeleton CreateSkeleton(const Node& rootNode);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	SkinCluster CreateSkinCluster(
		const ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData
	);

	Vector3 CalculateValue(const std::vector<KeyFrameVector3>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<KeyFrameQuaternion>& keyframes, float time);

	void SkeletonUpdate(Skeleton& skeleton);
	void SkinClusterUpdate(SkinCluster& skinCluster, const Skeleton& skeleton);
	void ApplyAnimation(Skeleton& skeleton, const AnimationData& animation, float animationTime);

	void MakeMaterialData();

protected:

	WorldTransform transform_;

private:

	std::unique_ptr<AnimationBase> animationBase_ = nullptr;
	Model* model_ = nullptr;

	AnimationData animationData_;
	float animationTime_ = 0.0f;

	SkinCluster skinCluster_;
	Skeleton skeleton_;

	std::unique_ptr<Line3d> line_;

	/*==================== マテリアル ====================*/

	// バッファリソース
	ComPtr<ID3D12Resource> materialResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	Material* materialData_ = nullptr;

	// 描画するか
	RenderOptions renderOptions_;

};