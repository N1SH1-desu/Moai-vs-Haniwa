#pragma once

#include <memory>
#include "Scene.h"
#include "Camera.h"
#include "FreeCameraController.h"
#include "Model.h"
#include "Effect.h"
#include "Sprite.h"
#include "Artifact.h"
// ゲームシーン
class SceneGame : public Scene
{
public:
	SceneGame();
	~SceneGame();
	
	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(float elapsedTime) override;

	int GameTimer;

	bool endBattle;

private:
	float Easing(float x)
	{
		return 1 - powf(1 - x, 4);
	}
	int	table;
	struct Object
	{
		bool					onGround = false;
		DirectX::XMFLOAT3		velocity = { 0, 0, 0 };
		DirectX::XMFLOAT3		position = { 0, 0, 0 };
		DirectX::XMFLOAT3		angle = { 0, 0, 0 };
		DirectX::XMFLOAT3		scale = { 1, 1, 1 };
		DirectX::XMFLOAT4X4		transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		std::unique_ptr<Model>	model;

		void UpdateTransform()
		{
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
			DirectX::XMMATRIX WorldTransform = S * R * T;
			DirectX::XMStoreFloat4x4(&transform, WorldTransform);
		}
		bool								death;
	};
	/*Camera								camera;
	FreeCameraController				cameraController;
	Object								player;
	Object								player2;*/
	Object								  ring;
	std::unique_ptr<Characters::Artifact> moai;
	std::unique_ptr<Characters::Artifact> haniwa;
	Sprite* sprText = nullptr;
	float								acceleration = 30.0f;
	float								deceleration = 20.0f;
	float								moveSpeed = 5.0f;
	float								turnSpeed = DirectX::XMConvertToRadians(720);
	float								downhillOffset = 43.0f;
	Effect* hitEffect=nullptr;
	// ANS①-①ヒットストップとカメラシェイクに必要な変数
	float								hitStopLastSeconds = 0.0f;
	float								hitStopSecondsLength = 0.3f;
	float								cameraShakeRange = 0.03f;

	std::unique_ptr<AudioSource>bgm;
	std::unique_ptr<AudioSource> SEPunch;
	
};


