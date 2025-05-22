#pragma once
#include <memory>
#include <list>
#include <string>

#include "GlobalVariables.h"
#include "Object3d.h"

#include "Wall.h"

class WallFactory : public Object3d
{
public:

	void Init();

	void Update();

	void Finalize();

private:

	// 調整項目の初期化
	void GlobalInit();

	// 敵を追加する
	void CreateObject();
	// 敵を削除する
	void DeleteObject();

private:

	// json保存
	GlobalVariables* global_ = GlobalVariables::GetInstance();

	// StageObjectを管理するlist
	std::list<std::unique_ptr<Wall>> wallObjects_;

	// 現在のWallの数を取得
	int32_t objectAddCount_ = 0;
	int32_t objectSubtractCount_ = 0;

};