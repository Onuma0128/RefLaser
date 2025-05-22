#pragma once

class DirectXEngine;

class ModelBase
{
private:
	static ModelBase* instance_;

	ModelBase() = default;
	~ModelBase() = default;
	ModelBase(ModelBase&) = delete;
	ModelBase& operator=(ModelBase&) = delete;

public:
	// シングルトンインスタンスの取得
	static ModelBase* GetInstance();

	void Initialize(DirectXEngine* dxEngine);

	void Finalize();


	// ゲッター
	DirectXEngine* GetDxEngine() const { return dxEngine_; }

private:
	DirectXEngine* dxEngine_ = nullptr;
};

