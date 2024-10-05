#include <imgui.h>
#include "Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "Collision.h"

// 初期化
void SceneGame::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	//カメラ
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// 画角
		screenWidth / screenHeight,			// 画面アスペクト比
		0.1f,								// ニアクリップ
		1000.0f								// ファークリップ
	);
	camera.SetLookAt(
		{ 13, 10, 27 },		// 視点
		{ 13, 1, 16 },		// 注視点
		{ 0, 1, 0 }			// 上ベクトル
	);
	cameraController.SyncCameraToController(camera);

	//ステージ
	stage.model = std::make_unique<Model>("");

	//プレイヤー初期化
	player.position = { 13, 5, 16 };
	player.scale = { 0.01f, 0.01f, 0.01f };

	player2.position = { 20, 5, 16 };
	player2.scale = { 0.01f, 0.01f, 0.01f };
	
	player.model = std::make_unique<Model>("");
	player2.model = std::make_unique<Model>("");
	
	cameraController.Update();
	cameraController.SyncControllerToCamera(camera);

}

// 終了化
void SceneGame::Finalize()
{
	//ゲージ終了化
	
	//カメラコントローラー終了化
	/*if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}*/
	//プレイヤー終了化
	/*if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}*/
	//ステージ終了化
	/*StageManager::Instance().Clear();*/
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// プレイヤー移動処理
	{
		// 入力処理
		float axisX = 0.0f;
		float axisY = 0.0f;
		if (GetAsyncKeyState('W') & 0x8000) axisY += 1.0f;
		if (GetAsyncKeyState('S') & 0x8000) axisY -= 1.0f;
		if (GetAsyncKeyState('D') & 0x8000) axisX += 1.0f;
		if (GetAsyncKeyState('A') & 0x8000) axisX -= 1.0f;
		if (GetAsyncKeyState(VK_SPACE) & 0x01) player.velocity.y = jumpSpeed;

		// カメラの方向
		const DirectX::XMFLOAT3& cameraFront = camera.GetFront();
		const DirectX::XMFLOAT3& camemraRight = camera.GetRight();
		float cameraFrontLengthXZ = sqrtf(cameraFront.x * cameraFront.x + cameraFront.z * cameraFront.z);
		float cameraRightLengthXZ = sqrtf(camemraRight.x * camemraRight.x + camemraRight.z * camemraRight.z);
		float cameraFrontX = cameraFront.x / cameraFrontLengthXZ;
		float cameraFrontZ = cameraFront.z / cameraFrontLengthXZ;
		float cameraRightX = camemraRight.x / cameraRightLengthXZ;
		float cameraRightZ = camemraRight.z / cameraRightLengthXZ;

		// 移動ベクトル
		float vecX = cameraFrontX * axisY + cameraRightX * axisX;
		float vecZ = cameraFrontZ * axisY + cameraRightZ * axisX;
		float vecLength = sqrtf(vecX * vecX + vecZ * vecZ);

		// 横方向移動処理
		if (vecLength > 0)
		{
			// 単位ベクトル化
			vecX /= vecLength;
			vecZ /= vecLength;

			// 加速処理
			float acceleration = this->acceleration * elapsedTime;
			player.velocity.x += vecX * acceleration;
			player.velocity.z += vecZ * acceleration;

			// 最大速度制限
			float velocityLength = sqrtf(player.velocity.x * player.velocity.x + player.velocity.z * player.velocity.z);
			if (velocityLength > moveSpeed)
			{
				player.velocity.x = (player.velocity.x / velocityLength) * moveSpeed;
				player.velocity.z = (player.velocity.z / velocityLength) * moveSpeed;
			}

			// 進行方向を向くようにする
			{
				// プレイヤーの向いている方向
				float frontX = sinf(player.angle.y);
				float frontZ = cosf(player.angle.y);

				// 回転量調整
				float dot = frontX * vecX + frontZ * vecZ;
				float rot = (std::min)(1.0f - dot, turnSpeed * elapsedTime);

				// 左右判定をして回転処理
				float cross = frontX * vecZ - frontZ * vecX;
				if (cross < 0.0f)
				{
					player.angle.y += rot;
				}
				else
				{
					player.angle.y -= rot;
				}
			}
		}
		else
		{
			// 減速処理
			float deceleration = this->deceleration * elapsedTime;
			float velocityLength = sqrtf(player.velocity.x * player.velocity.x + player.velocity.z * player.velocity.z);
			if (velocityLength > deceleration)
			{
				player.velocity.x -= (player.velocity.x / velocityLength) * deceleration;
				player.velocity.z -= (player.velocity.z / velocityLength) * deceleration;
			}
			else
			{
				player.velocity.x = 0.0f;
				player.velocity.z = 0.0f;
			}
		}

		// 重力処理
		player.velocity.y -= gravity * elapsedTime;

		// 移動量
		float moveX = player.velocity.x * elapsedTime;
		float moveY = player.velocity.y * elapsedTime;
		float moveZ = player.velocity.z * elapsedTime;

		// 接地処理
		{
			if (player.position.y < 0.0f)
			{
				player.position.y = 0;
			}
			if (player2.position.y < 0.0f)
			{
				player2.position.y = 0;
			}
		}

		// 移動
		player.position.x += moveX;
		player.position.y += moveY;
		player.position.z += moveZ;

	}
	////ステージ更新処理
	//StageManager::Instance().Update(elapsedTime);
	// プレイヤー行列更新処理
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(player.scale.x, player.scale.y, player.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(player.angle.x, player.angle.y, player.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(player.position.x, player.position.y, player.position.z);
		DirectX::XMStoreFloat4x4(&player.transform, S* R* T);
	}
	////エフェクト更新処理
	//EffectManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render(float elapsedtime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	// モデル描画
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &camera;
	modelRenderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, player.transform, player.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, player2.transform, player2.model.get(), ShaderId::Lambert);
	
}
