#include <imgui.h>
#include <ImGuizmo.h>
#include "Graphics.h"
#include "Collision.h"
#include "SceneGame.h"

// 初期化
 SceneGame::SceneGame()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	end = 0;
	// カメラ設定
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

	// モデル
	stage.model = std::make_unique<Model>("Data/Model/Greybox/Greybox.mdl");

	player.position = { 13, 1, 16 };
	player.scale = { 0.01f, 0.01f, 0.01f };
	player.model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
	
	player2.position = { 11, 3, 16 };
	player2.scale = { 0.01f, 0.01f, 0.01f };
	player2.model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
	
	
}

// 終了化
//void SceneGame::Finalize()
//{
//	//ゲージ終了化
//	
//	//カメラコントローラー終了化
//	/*if (cameraController != nullptr)
//	{
//		delete cameraController;
//		cameraController = nullptr;
//	}*/
//	//プレイヤー終了化
//	/*if (player != nullptr)
//	{
//		delete player;
//		player = nullptr;
//	}*/
//	//ステージ終了化
//	/*StageManager::Instance().Clear();*/
//}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	cameraController.Update();
	cameraController.SyncControllerToCamera(camera);
	
	// プレイヤー移動処理
	{
		// 入力処理
		float axisX = 0.0f;
		float axisY = 0.0f;
		if (GetAsyncKeyState('W') & 0x8000) axisY += 1.0f;
		if (GetAsyncKeyState('S') & 0x8000) axisY -= 1.0f;
		if (GetAsyncKeyState('D') & 0x8000) axisX += 1.0f;
		if (GetAsyncKeyState('A') & 0x8000) axisX -= 1.0f;
		
		float axis2X = 0.0f;
		float axis2Y = 0.0f;
		if (GetAsyncKeyState('I') & 0x8000) axis2Y += 1.0f;
		if (GetAsyncKeyState('K') & 0x8000) axis2Y -= 1.0f;
		if (GetAsyncKeyState('L') & 0x8000) axis2X += 1.0f;
		if (GetAsyncKeyState('J') & 0x8000) axis2X -= 1.0f;


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

		float vec2X = cameraFrontX * axis2Y + cameraRightX * axis2X;
		float vec2Z = cameraFrontZ * axis2Y + cameraRightZ * axis2X;
		float vec2Length = sqrtf(vec2X * vec2X + vec2Z * vec2Z);

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

		if (vec2Length > 0)
		{
			// 単位ベクトル化
			vec2X /= vec2Length;
			vec2Z /= vec2Length;

			// 加速処理
			float acceleration = this->acceleration * elapsedTime;
			player2.velocity.x += vec2X * acceleration;
			player2.velocity.z += vec2Z * acceleration;

			// 最大速度制限
			float velocityLength = sqrtf(player2.velocity.x * player2.velocity.x + player2.velocity.z * player2.velocity.z);
			if (velocityLength > moveSpeed)
			{
				player2.velocity.x = (player2.velocity.x / velocityLength) * moveSpeed;
				player2.velocity.z = (player2.velocity.z / velocityLength) * moveSpeed;
			}

			// 進行方向を向くようにする
			{
				// プレイヤーの向いている方向
				float frontX = sinf(player2.angle.y);
				float frontZ = cosf(player2.angle.y);

				// 回転量調整
				float dot = frontX * vec2X + frontZ * vec2Z;
				float rot = (std::min)(1.0f - dot, turnSpeed * elapsedTime);

				// 左右判定をして回転処理
				float cross = frontX * vec2Z - frontZ * vec2X;
				if (cross < 0.0f)
				{
					player2.angle.y += rot;
				}
				else
				{
					player2.angle.y -= rot;
				}
			}
		}
		else
		{
			// 減速処理
			float deceleration = this->deceleration * elapsedTime;
			float velocityLength = sqrtf(player2.velocity.x * player2.velocity.x + player2.velocity.z * player2.velocity.z);
			if (velocityLength > deceleration)
			{
				player2.velocity.x -= (player2.velocity.x / velocityLength) * deceleration;
				player2.velocity.z -= (player2.velocity.z / velocityLength) * deceleration;
			}
			else
			{
				player2.velocity.x = 0.0f;
				player2.velocity.z = 0.0f;
			}
		}

		// 重力処理
		//player.velocity.y -= gravity * elapsedTime;

		// 移動量
		float moveX = player.velocity.x * elapsedTime;
		float moveY = player.velocity.y * elapsedTime;
		float moveZ = player.velocity.z * elapsedTime;

		float move2X = player2.velocity.x * elapsedTime;
		float move2Y = player2.velocity.y * elapsedTime;
		float move2Z = player2.velocity.z * elapsedTime;

		// 接地処理
		{
			if (player.position.y < 1.0f)
			{
				player.position.y = 1;
			}
			if (player2.position.y < 1.0f)
			{
				player2.position.y = 1;
			}
		}

		// 移動
		player.position.x += moveX;
		player.position.y += moveY;
		player.position.z += moveZ;

		player2.position.x += move2X;
		player2.position.y += move2Y;
		player2.position.z += move2Z;

	}

	// プレイヤー行列更新処理
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(player.scale.x, player.scale.y, player.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(player.angle.x, player.angle.y, player.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(player.position.x, player.position.y, player.position.z);
		DirectX::XMStoreFloat4x4(&player.transform, S* R* T);
	}

	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(player2.scale.x, player2.scale.y, player2.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(player2.angle.x, player2.angle.y, player2.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(player2.position.x, player2.position.y, player2.position.z);
		DirectX::XMStoreFloat4x4(&player2.transform, S* R* T);
	}
	if (player.death)end = 1;
	if (player2.death)end = 2;
	
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
