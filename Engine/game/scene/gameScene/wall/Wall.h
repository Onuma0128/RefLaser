#pragma once
#include <string>

#include "GlobalVariables.h"
#include "Object3d.h"
#include "Collider.h"

class Wall :public Object3d, Collider
{
public:

	void Finalize();

	void Init();

	void Update();

	void OnCollisionEnter(Collider* other) override;
	void OnCollisionStay(Collider* other) override;
	void OnCollisionExit(Collider* other) override;

	// jsonで保存時の名前
	std::string GetGroupName()const { return groupName_; }
	// jsonの番号
	int32_t GetGroupNumber()const { return groupNumber_; }
	void SetGroupNumber(int32_t number) {
		groupNumber_ = number;
		// 番号が変わったので名前も変える
		groupName_ = "WallObject" + std::to_string(groupNumber_);
	}

	// エディター上の削除フラグ
	bool GetDelete()const { return isDelete_; }

private:

	// 調整項目の初期化
	void GlobalInit();
	// デバッグ用
	void DebugTransform();


private:

	GlobalVariables* global_ = GlobalVariables::GetInstance();

	// jsonで保存時の名前
	std::string groupName_;
	// jsonの番号
	int32_t groupNumber_;
	// 削除するか
	bool isDelete_;

};

