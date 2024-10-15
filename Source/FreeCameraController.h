#pragma once
#include "Camera.h"
#include "Input\GamePad.h"

class FreeCameraController
{
public:	
	// カメラからコントローラーへパラメータを同期する
	void SyncCameraToController(const Camera& camera);

	// コントローラーからカメラへパラメータを同期する
	void SyncControllerToCamera(Camera& camera);

	// 更新処理
	void Update();

	// ゲームパッドと連動
	void GetGamePad(GamePad* ref) { gamePad = ref; }

private:
	GamePad* gamePad = nullptr;

	DirectX::XMFLOAT3		eye;
	DirectX::XMFLOAT3		focus;
	DirectX::XMFLOAT3		up;
	DirectX::XMFLOAT3		right;
	float					distance;

	float					angleX;
	float					angleY;
};
