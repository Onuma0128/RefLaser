#pragma once

#include <memory>

#include "Camera.h"
#include "GlobalVariables.h"

class Player;

class GameCamera
{
public:

	void Init();

	void GlobalInit();

	void Update();
	void SabUpdate();

	void SetShake(float shakeStrength) { shakeStrength_ = shakeStrength; }

	void SetPlayer(Player* player) { player_ = player; }

	float LerpShortAngle(float a, float b, float t);

	Camera* GetCamera() { return mainCamera_.get(); }

private:

	GlobalVariables* global_ = GlobalVariables::GetInstance();

	Player* player_ = nullptr;

	std::unique_ptr<Camera> mainCamera_ = nullptr;
	std::unique_ptr<Camera> sabCamera_ = nullptr;

	float shakeStrength_ = 0.0f;
	float shakeDecay_ = 0.9f;   

};