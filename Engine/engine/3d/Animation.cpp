#include "Animation.h"

#include "assimp//Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "DirectXEngine.h"
#include "SrvManager.h"
#include "TextureManager.h"
#include "DeltaTimer.h"

#include "Object3d.h"
#include "Model.h"
#include "ModelManager.h"
#include "LightManager.h"
#include "CameraManager.h"
#include "Line3d.h"

#include "CreateBufferResource.h"

void Animation::Init(const std::string& directoryPath, const std::string& filename)
{
	ModelManager::GetInstance()->LoadModel(directoryPath, filename);
	animationBase_ = std::make_unique<AnimationBase>();
	animationBase_->Initialize();

	transform_ = WorldTransform();
	SetModel(filename);
	MakeMaterialData();

	animationData_ = LoadAnimationFile(directoryPath, filename);

	skeleton_ = CreateSkeleton(model_->GetModelData().rootNode);
	skinCluster_ = CreateSkinCluster(DirectXEngine::GetDevice(), skeleton_, model_->GetModelData());

	std::vector<Vector3> linePositions{};
	for (const Joint& joint : skeleton_.joints) {
		if (joint.parent) {
			// 初期の位置を取得
			Matrix4x4 parentMatrix = transform_.matWorld_ * skeleton_.joints[*joint.parent].skeletonSpaceMatrix;
			Matrix4x4 jointMatrix = transform_.matWorld_ * joint.skeletonSpaceMatrix;

			Vector3 parentPos = Vector3{}.Transform(parentMatrix);
			Vector3 jointPos = Vector3{}.Transform(jointMatrix);

			linePositions.push_back(parentPos);
			linePositions.push_back(jointPos);
		}
	}

	// Line3dを初期化
	line_ = std::make_unique<Line3d>();
	line_->Initialize(linePositions);
}

void Animation::SetSceneRenderer()
{
	renderOptions_ = {
		.enabled = true,
		.offscreen = true
	};
	DirectXEngine::GetSceneRenderer()->SetDrawList(this);
}

void Animation::Update()
{
	animationTime_ += DeltaTimer::GetDeltaTime();
	animationTime_ = std::fmod(animationTime_, animationData_.duration);
	
	ApplyAnimation(skeleton_, animationData_, animationTime_);
	SkeletonUpdate(skeleton_);
	SkinClusterUpdate(skinCluster_, skeleton_);

	transform_.TransferMatrix(Matrix4x4::Identity());

	int32_t count = 0;
	std::vector<Vector3> linePositions{};
	for (const Joint& joint : skeleton_.joints) {
		if (joint.parent) {
			// 初期の位置を取得
			Matrix4x4 parentMatrix = skeleton_.joints[*joint.parent].skeletonSpaceMatrix * transform_.matWorld_;
			Matrix4x4 jointMatrix = joint.skeletonSpaceMatrix * transform_.matWorld_;

			Vector3 parentPos = Vector3{}.Transform(parentMatrix);
			Vector3 jointPos = Vector3{}.Transform(jointMatrix);

			linePositions.push_back(parentPos);
			linePositions.push_back(jointPos);

			++count;
		}
	}

	line_->SetPositions(linePositions);
	line_->Update();
}

void Animation::Draw()
{
	animationBase_->DrawBase();

	auto commandList = DirectXEngine::GetCommandList();
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, transform_.GetConstBuffer()->GetGPUVirtualAddress());
	SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(3, skinCluster_.srvHandleIndex);
	commandList->SetGraphicsRootConstantBufferView(4, LightManager::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(5, LightManager::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(6, LightManager::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(8, CameraManager::GetInstance()->GetCameraResource()->GetGPUVirtualAddress());

	if (model_) {
		D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			model_->GetVertexBuffer(),
			skinCluster_.influenceBufferView
		};
		commandList->IASetVertexBuffers(0, 2, vbvs);
		model_->Draw(true);
	}
}

AnimationData Animation::LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
{
	AnimationData animationData;
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0);
	aiAnimation* animationAssimp = scene->mAnimations[0];
	animationData.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animationData.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		
		// 座標
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyFrameVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}
		// 回転
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyFrameQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w };
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}
		// スケール
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyFrameVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
	}

	return animationData;
}

Skeleton Animation::CreateSkeleton(const Node& rootNode)
{
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	return skeleton;
}

int32_t Animation::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = Matrix4x4::Identity();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());
	joint.parent = parent;
	joints.push_back(joint);
	for (const Node& child : node.children) {
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}

	return joint.index;
}

SkinCluster Animation::CreateSkinCluster(const ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelData& modelData)
{
	SkinCluster skinCluster;
	SrvManager* srvManager = SrvManager::GetInstance();

	// **パレットリソースの作成**
	size_t paletteSize = skeleton.joints.size() * sizeof(WellForGPU);
	skinCluster.paletteResource = CreateBufferResource(device, paletteSize).Get();

	// **パレットのマッピング**
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = std::span<WellForGPU>(mappedPalette, skeleton.joints.size());

	// **Influenceリソースの作成**
	size_t influenceSize = modelData.vertices.size() * sizeof(VertexInfluence);
	skinCluster.infuenceResource = CreateBufferResource(device, influenceSize).Get();

	// **Influenceのマッピング**
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.infuenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, influenceSize);
	skinCluster.mappedInfluence = std::span<VertexInfluence>(mappedInfluence, modelData.vertices.size());

	// **Influence バッファビューの設定**
	skinCluster.influenceBufferView.BufferLocation = skinCluster.infuenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = static_cast<UINT>(influenceSize);
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	// **SRVの作成**
	skinCluster.srvHandleIndex = srvManager->Allocate() + TextureManager::kSRVIndexTop; // 空きSRVスロットを取得
	srvManager->CreateSRVforStructuredBuffer(skinCluster.srvHandleIndex, skinCluster.paletteResource.Get(), static_cast<UINT>(skeleton.joints.size()), sizeof(WellForGPU));

	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
	for (size_t i = 0; i < skinCluster.inverseBindPoseMatrices.size(); ++i) {
		skinCluster.inverseBindPoseMatrices[i] = Matrix4x4::Identity();
	}

	for (const auto& jointWeight : model_->GetModelData().skinClusterData) {
		// jointWeight.firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
		auto it = skeleton.jointMap.find(jointWeight.first);
		if (it == skeleton.jointMap.end()) { // そんな名前のjointは存在しない。なので次に回す
			continue;
		}
		// (*it).secondにはjointのindexが入っているので、該当のindexのinverseBindPoseMatrixを代入
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPosMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
			// 空いている場所に入れる
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
				// weight == 0.0fが開いている状態なので、その場所にweightとjointのindexを代入
				if (currentInfluence.weights[index] == 0.0f) {
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}

	return skinCluster;
}

void Animation::SkeletonUpdate(Skeleton& skeleton)
{
	for (Joint& joint : skeleton.joints) {
		joint.localMatrix = Matrix4x4::Affine(joint.transform.scale, joint.transform.rotation, joint.transform.translation);
		if (joint.parent) {
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
		} else {
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void Animation::SkinClusterUpdate(SkinCluster& skinCluster, const Skeleton& skeleton)
{
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());
		skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix =
			skinCluster.inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
		skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
			Matrix4x4::Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix).Transpose();
	}
}

void Animation::ApplyAnimation(Skeleton& skeleton, const AnimationData& animation, float animationTime)
{
	for (Joint& joint : skeleton.joints) {
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translation = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotation = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}
	}
}

void Animation::SetModel(const std::string& filePath)
{
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}

void Animation::SetTexture(const std::string& directoryPath, const std::string& filePath)
{
	model_->SetTexture(directoryPath, filePath);
}

void Animation::SetColor(const Vector4& color)
{
	materialData_->color = color;
}

Vector3 Animation::CalculateValue(const std::vector<KeyFrameVector3>& keyframes, float time)
{
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time = keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Vector3::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}

Quaternion Animation::CalculateValue(const std::vector<KeyFrameQuaternion>& keyframes, float time)
{
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Quaternion::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}

void Animation::MakeMaterialData()
{
	// マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = CreateBufferResource(DirectXEngine::GetDevice(), sizeof(Material)).Get();
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は白を書き込んでいく
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = true;
	materialData_->uvTransform = Matrix4x4::Identity();
	materialData_->shininess = 20.0f;
	materialData_->environmentCoefficient = 0;
}
