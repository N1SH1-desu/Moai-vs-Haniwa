#include <imgui.h>
#include <ImGuizmo.h>
#include "Graphics.h"
#include "Collision.h"
#include "Scene/LandWalkScene.h"

// �R���X�g���N�^
LandWalkScene::LandWalkScene()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// �J�����ݒ�
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// ��p
		screenWidth / screenHeight,			// ��ʃA�X�y�N�g��
		0.1f,								// �j�A�N���b�v
		1000.0f								// �t�@�[�N���b�v
	);
	camera.SetLookAt(
		{ 13, 10, 27 },		// ���_
		{ 13, 1, 16 },		// �����_
		{ 0, 1, 0 }			// ��x�N�g��
	);
	cameraController.SyncCameraToController(camera);

	// ���f��
	stage.model = std::make_unique<Model>("Data/Model/Greybox/Greybox.mdl");

	//�v���C���[���f��
	player.position = { 13, 5, 16 };
	player.scale = { 0.01f, 0.01f, 0.01f };
	player.model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	//�G�l�~�[���f��
	enemy.position = { 15, 5, 16 };
	enemy.scale = { 0.01f, 0.01f, 0.01f };
	enemy.model = std::make_unique<Model>("Data/Model/Slime/Slime.mdl");
}

// �X�V����
void LandWalkScene::Update(float elapsedTime)
{
	// �J�����X�V����
	cameraController.Update();
	cameraController.SyncControllerToCamera(camera);

	//for (LandWalkScene* enemy : enemies)
	//{
	//	enemy->Update(elapsedTime);
	//}

	////�j������
	////enemies�͈̔�for������erase()����ƕs����������Ă��܂����߁A
	////�X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	//for (LandWalkScene* enemy : removes)
	//{
	//	//std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
	//	std::vector<LandWalkScene*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
	//	if (it != enemies.end())
	//	{
	//		enemies.erase(it);
	//	}

	//	//�폜
	//	delete enemy;
	//}
	////�j�����X�g���N���A
	//removes.clear();

	//// �G���m�̏Փˏ���
	//CollisionPlayerVsEnemies();

	// �v���C���[�ړ�����
	{
		// ���͏���
		float axisX = 0.0f;
		float axisY = 0.0f;
		if (GetAsyncKeyState('W') & 0x8000) axisY += 1.0f;
		if (GetAsyncKeyState('S') & 0x8000) axisY -= 1.0f;
		if (GetAsyncKeyState('D') & 0x8000) axisX += 1.0f;
		if (GetAsyncKeyState('A') & 0x8000) axisX -= 1.0f;
		//if (GetAsyncKeyState(VK_SPACE) & 0x01) player.velocity.y = jumpSpeed;

		// �J�����̕���
		const DirectX::XMFLOAT3& cameraFront = camera.GetFront();
		const DirectX::XMFLOAT3& camemraRight = camera.GetRight();
		float cameraFrontLengthXZ = sqrtf(cameraFront.x * cameraFront.x + cameraFront.z * cameraFront.z);
		float cameraRightLengthXZ = sqrtf(camemraRight.x * camemraRight.x + camemraRight.z * camemraRight.z);
		float cameraFrontX = cameraFront.x / cameraFrontLengthXZ;
		float cameraFrontZ = cameraFront.z / cameraFrontLengthXZ;
		float cameraRightX = camemraRight.x / cameraRightLengthXZ;
		float cameraRightZ = camemraRight.z / cameraRightLengthXZ;

		// �ړ��x�N�g��
		float vecX = cameraFrontX * axisY + cameraRightX * axisX;
		float vecZ = cameraFrontZ * axisY + cameraRightZ * axisX;
		float vecLength = sqrtf(vecX * vecX + vecZ * vecZ);

		// �������ړ�����
		if (vecLength > 0)
		{
			// �P�ʃx�N�g����
			vecX /= vecLength;
			vecZ /= vecLength;

			// ��������
			float acceleration = this->acceleration * elapsedTime;
			player.velocity.x += vecX * acceleration;
			player.velocity.z += vecZ * acceleration;

			// �ő呬�x����
			float velocityLength = sqrtf(player.velocity.x * player.velocity.x + player.velocity.z * player.velocity.z);
			if (velocityLength > moveSpeed)
			{
				player.velocity.x = (player.velocity.x / velocityLength) * moveSpeed;
				player.velocity.z = (player.velocity.z / velocityLength) * moveSpeed;
			}

			// �i�s�����������悤�ɂ���
			{
				// �v���C���[�̌����Ă������
				float frontX = sinf(player.angle.y);
				float frontZ = cosf(player.angle.y);

				// ��]�ʒ���
				float dot = frontX * vecX + frontZ * vecZ;
				float rot = (std::min)(1.0f - dot, turnSpeed * elapsedTime);

				// ���E��������ĉ�]����
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
			// ��������
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

		// �d�͏���
		//player.velocity.y -= gravity * elapsedTime;

		// �ړ���
		float moveX = player.velocity.x * elapsedTime;
		float moveY = player.velocity.y * elapsedTime;
		float moveZ = player.velocity.z * elapsedTime;
		
		// �ڒn����
		{

		}

		// �ړ�
		player.position.x += moveX;
		player.position.y += moveY;
		player.position.z += moveZ;
		
	}

	// �v���C���[�s��X�V����
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(player.scale.x, player.scale.y, player.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(player.angle.x, player.angle.y, player.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(player.position.x, player.position.y, player.position.z);
		DirectX::XMStoreFloat4x4(&player.transform, S * R * T);
	}

	// �G�l�~�[�ړ�����
	{
		// ���͏���
		float axisX = 0.0f;
		float axisY = 0.0f;
		if (GetAsyncKeyState('I') & 0x8000) axisY += 1.0f;
		if (GetAsyncKeyState('K') & 0x8000) axisY -= 1.0f;
		if (GetAsyncKeyState('L') & 0x8000) axisX += 1.0f;
		if (GetAsyncKeyState('J') & 0x8000) axisX -= 1.0f;

		// �J�����̕���
		const DirectX::XMFLOAT3& cameraFront = camera.GetFront();
		const DirectX::XMFLOAT3& camemraRight = camera.GetRight();
		float cameraFrontLengthXZ = sqrtf(cameraFront.x * cameraFront.x + cameraFront.z * cameraFront.z);
		float cameraRightLengthXZ = sqrtf(camemraRight.x * camemraRight.x + camemraRight.z * camemraRight.z);
		float cameraFrontX = cameraFront.x / cameraFrontLengthXZ;
		float cameraFrontZ = cameraFront.z / cameraFrontLengthXZ;
		float cameraRightX = camemraRight.x / cameraRightLengthXZ;
		float cameraRightZ = camemraRight.z / cameraRightLengthXZ;

		// �ړ��x�N�g��
		float vecX = cameraFrontX * axisY + cameraRightX * axisX;
		float vecZ = cameraFrontZ * axisY + cameraRightZ * axisX;
		float vecLength = sqrtf(vecX * vecX + vecZ * vecZ);

		// �������ړ�����
		if (vecLength > 0)
		{
			// �P�ʃx�N�g����
			vecX /= vecLength;
			vecZ /= vecLength;

			// ��������
			float acceleration = this->acceleration * elapsedTime;
			enemy.velocity.x += vecX * acceleration;
			enemy.velocity.z += vecZ * acceleration;

			// �ő呬�x����
			float velocityLength = sqrtf(enemy.velocity.x * enemy.velocity.x + enemy.velocity.z * enemy.velocity.z);
			if (velocityLength > moveSpeed)
			{
				enemy.velocity.x = (enemy.velocity.x / velocityLength) * moveSpeed;
				enemy.velocity.z = (enemy.velocity.z / velocityLength) * moveSpeed;
			}

			// �i�s�����������悤�ɂ���
			{
				// �v���C���[�̌����Ă������
				float frontX = sinf(enemy.angle.y);
				float frontZ = cosf(enemy.angle.y);

				// ��]�ʒ���
				float dot = frontX * vecX + frontZ * vecZ;
				float rot = (std::min)(1.0f - dot, turnSpeed * elapsedTime);

				// ���E��������ĉ�]����
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
			// ��������
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

		// �d�͏���
		//player.velocity.y -= gravity * elapsedTime;

		// �ړ���
		float moveX = enemy.velocity.x * elapsedTime;
		float moveY = enemy.velocity.y * elapsedTime;
		float moveZ = enemy.velocity.z * elapsedTime;

		// �ڒn����
		{

		}

		// �ړ�
		enemy.position.x += moveX;
		enemy.position.y += moveY;
		enemy.position.z += moveZ;

	}

	// �G�l�~�[�s��X�V����
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(enemy.scale.x, enemy.scale.y, enemy.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(enemy.angle.x, enemy.angle.y, enemy.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(enemy.position.x, enemy.position.y, enemy.position.z);
		DirectX::XMStoreFloat4x4(&enemy.transform, S* R* T);
	}


	// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
	//void LandWalkScene::CollisionPlayerVsEnemies()
	//{
	//	EnemyManager& enemyManager = EnemyManager::Instance();

	//	// �S�Ă̓G�Ƒ�������ŏՓˏ���
	//	int enemyCount = enemyManager.GetEnemyCount();
	//	for (int i = 0; i < enemyCount; ++i)
	//	{
	//		Enemy* enemy = enemyManager.GetEnemy(i);

	//	// �Փˏ���
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
	//			////�����o����̈ʒu�ݒ�
	//			//enemy->SetPosition(outPosition);

	//			//�G�̐^��t�߂ɓ����������ǂ����𔻒�
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

// �`�揈��
void LandWalkScene::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();
	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();

	// ���f���`��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &camera;
	modelRenderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, player.transform, player.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, enemy.transform, enemy.model.get(), ShaderId::Lambert);


	//3D�f�o�b�O
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

// GUI�`�揈��
void LandWalkScene::DrawGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();

	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);

	if (ImGui::Begin(u8"�n����s", nullptr, ImGuiWindowFlags_NoNavInputs))
	{
		ImGui::Text(u8"�ړ�����FWASD");
		ImGui::Text(u8"�W�����v����FSpace");
		if (ImGui::Button(u8"���Z�b�g"))
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


