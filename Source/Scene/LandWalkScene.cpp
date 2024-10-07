#include <imgui.h>
#include <ImGuizmo.h>
#include "Graphics.h"
#include "Collision.h"
#include "Scene/LandWalkScene.h"

// コンストラクタ
LandWalkScene::LandWalkScene()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

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

	//プレイヤーモデル
	player.position = { 13, 5, 16 };
	player.scale = { 0.01f, 0.01f, 0.01f };
	player.model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	//エネミーモデル
	enemy.position = { 15, 5, 16 };
	enemy.scale = { 0.01f, 0.01f, 0.01f };
	enemy.model = std::make_unique<Model>("Data/Model/Slime/Slime.mdl");
}

// 更新処理
void LandWalkScene::Update(float elapsedTime)
{
	// カメラ更新処理
	cameraController.Update();
	cameraController.SyncControllerToCamera(camera);

	//for (LandWalkScene* enemy : enemies)
	//{
	//	enemy->Update(elapsedTime);
	//}

	////破棄処理
	////enemiesの範囲for文中でerase()すると不具合が発生してしまうため、
	////更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	//for (LandWalkScene* enemy : removes)
	//{
	//	//std::vectorから要素を削除する場合はイテレーターで削除しなければならない
	//	std::vector<LandWalkScene*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
	//	if (it != enemies.end())
	//	{
	//		enemies.erase(it);
	//	}

	//	//削除
	//	delete enemy;
	//}
	////破棄リストをクリア
	//removes.clear();

	//// 敵同士の衝突処理
	//CollisionPlayerVsEnemies();

	// プレイヤー移動処理
	{
		// 入力処理
		float axisX = 0.0f;
		float axisY = 0.0f;
		if (GetAsyncKeyState('W') & 0x8000) axisY += 1.0f;
		if (GetAsyncKeyState('S') & 0x8000) axisY -= 1.0f;
		if (GetAsyncKeyState('D') & 0x8000) axisX += 1.0f;
		if (GetAsyncKeyState('A') & 0x8000) axisX -= 1.0f;
		//if (GetAsyncKeyState(VK_SPACE) & 0x01) player.velocity.y = jumpSpeed;

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
		//player.velocity.y -= gravity * elapsedTime;

		// 移動量
		float moveX = player.velocity.x * elapsedTime;
		float moveY = player.velocity.y * elapsedTime;
		float moveZ = player.velocity.z * elapsedTime;
		
		// 接地処理
		{

		}

		// 移動
		player.position.x += moveX;
		player.position.y += moveY;
		player.position.z += moveZ;
		
	}

	// プレイヤー行列更新処理
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(player.scale.x, player.scale.y, player.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(player.angle.x, player.angle.y, player.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(player.position.x, player.position.y, player.position.z);
		DirectX::XMStoreFloat4x4(&player.transform, S * R * T);
	}

	// エネミー移動処理
	{
		// 入力処理
		float axisX = 0.0f;
		float axisY = 0.0f;
		if (GetAsyncKeyState('I') & 0x8000) axisY += 1.0f;
		if (GetAsyncKeyState('K') & 0x8000) axisY -= 1.0f;
		if (GetAsyncKeyState('L') & 0x8000) axisX += 1.0f;
		if (GetAsyncKeyState('J') & 0x8000) axisX -= 1.0f;

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
			enemy.velocity.x += vecX * acceleration;
			enemy.velocity.z += vecZ * acceleration;

			// 最大速度制限
			float velocityLength = sqrtf(enemy.velocity.x * enemy.velocity.x + enemy.velocity.z * enemy.velocity.z);
			if (velocityLength > moveSpeed)
			{
				enemy.velocity.x = (enemy.velocity.x / velocityLength) * moveSpeed;
				enemy.velocity.z = (enemy.velocity.z / velocityLength) * moveSpeed;
			}

			// 進行方向を向くようにする
			{
				// プレイヤーの向いている方向
				float frontX = sinf(enemy.angle.y);
				float frontZ = cosf(enemy.angle.y);

				// 回転量調整
				float dot = frontX * vecX + frontZ * vecZ;
				float rot = (std::min)(1.0f - dot, turnSpeed * elapsedTime);

				// 左右判定をして回転処理
				float cross = frontX * vecZ - frontZ * vecX;
				if (cross < 0.0f)
				{
					enemy.angle.y += rot;
				}
				else
				{
					enemy.angle.y -= rot;
				}
			}
		}
		else
		{
			// 減速処理
			float deceleration = this->deceleration * elapsedTime;
			float velocityLength = sqrtf(enemy.velocity.x * enemy.velocity.x + enemy.velocity.z * enemy.velocity.z);
			if (velocityLength > deceleration)
			{
				enemy.velocity.x -= (enemy.velocity.x / velocityLength) * deceleration;
				enemy.velocity.z -= (enemy.velocity.z / velocityLength) * deceleration;
			}
			else
			{
				enemy.velocity.x = 0.0f;
				enemy.velocity.z = 0.0f;
			}
		}

		// 重力処理
		//player.velocity.y -= gravity * elapsedTime;

		// 移動量
		float moveX = enemy.velocity.x * elapsedTime;
		float moveY = enemy.velocity.y * elapsedTime;
		float moveZ = enemy.velocity.z * elapsedTime;

		// 接地処理
		{

		}

		// 移動
		enemy.position.x += moveX;
		enemy.position.y += moveY;
		enemy.position.z += moveZ;

	}

	// エネミー行列更新処理
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(enemy.scale.x, enemy.scale.y, enemy.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(enemy.angle.x, enemy.angle.y, enemy.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(enemy.position.x, enemy.position.y, enemy.position.z);
		DirectX::XMStoreFloat4x4(&enemy.transform, S* R* T);
	}


	// プレイヤーとエネミーとの衝突処理
	//void LandWalkScene::CollisionPlayerVsEnemies()
	//{
	//	EnemyManager& enemyManager = EnemyManager::Instance();

	//	// 全ての敵と総当たりで衝突処理
	//	int enemyCount = enemyManager.GetEnemyCount();
	//	for (int i = 0; i < enemyCount; ++i)
	//	{
	//		Enemy* enemy = enemyManager.GetEnemy(i);

	//	// 衝突処理
	//		DirectX::XMFLOAT3 outPosition;
	//		if (Collision::IntersectCylinderVsCylinder(
	//			position,
	//			radius,
	//			height,
	//			enemy->GetPosition(),
	//			enemy->GetRadius(),
	//			enemy->GetHeight(),
	//			outPosition))
	//		{
	//			////押し出し後の位置設定
	//			//enemy->SetPosition(outPosition);

	//			//敵の真上付近に当たったかどうかを判定
	//			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
	//			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
	//			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
	//			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
	//			DirectX::XMFLOAT3 normal;
	//			DirectX::XMStoreFloat3(&normal, N);
	//			
	//		}
	//	}
	//}
}

// 描画処理
void LandWalkScene::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();
	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();

	// モデル描画
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &camera;
	modelRenderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, player.transform, player.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, enemy.transform, enemy.model.get(), ShaderId::Lambert);


	//3Dデバッグ
	DirectX::XMFLOAT3 p;
	DirectX::XMFLOAT3 e;

	p.x = player.position.x;
	p.y = player.position.y + characterHitOffset;
	p.z = player.position.z;
	shapeRenderer->DrawSphere(p, characterHitRadius, { 1,1,1,1 });
	shapeRenderer->Render(dc, camera.GetView(), camera.GetProjection());

	e.x = enemy.position.x;
	e.y = enemy.position.y + characterHitOffset;
	e.z = enemy.position.z;
	shapeRenderer->DrawSphere(e, characterHitRadius, { 1,1,1,1 });
	shapeRenderer->Render(dc, camera.GetView(), camera.GetProjection());
}

// GUI描画処理
void LandWalkScene::DrawGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();

	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);

	if (ImGui::Begin(u8"地上歩行", nullptr, ImGuiWindowFlags_NoNavInputs))
	{
		ImGui::Text(u8"移動操作：WASD");
		ImGui::Text(u8"ジャンプ操作：Space");
		if (ImGui::Button(u8"リセット"))
		{
			player.position = { 13, 5, 16 };
			player.velocity = { 0, 0, 0 };
			enemy.position = { 13, 5, 16 };
			enemy.velocity = { 0, 0, 0 };
		}

		ImGui::DragFloat3("Position", &player.position.x, 0.1f);
		ImGui::DragFloat3("Velocity", &player.velocity.x, 0.1f);
		ImGui::Checkbox("OnGround", &player.onGround);
		ImGui::DragFloat("Gravity", &gravity, 0.1f);
		ImGui::DragFloat("Acceleration", &acceleration, 0.1f);
		ImGui::DragFloat("Deceleration", &deceleration, 0.1f);
		ImGui::DragFloat("MoveSpeed", &moveSpeed, 0.1f);
		ImGui::DragFloat("TurnSpeed", &turnSpeed, 0.1f);
		ImGui::DragFloat("JumpSpeed", &jumpSpeed, 0.1f);
		ImGui::DragFloat("DownhillOffset", &downhillOffset, 0.01f);
	}
	ImGui::End();
}


