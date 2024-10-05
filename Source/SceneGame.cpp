#include <imgui.h>
#include "Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "Collision.h"

// ������
void SceneGame::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	//�J����
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

	//�X�e�[�W
	stage.model = std::make_unique<Model>("");

	//�v���C���[������
	player.position = { 13, 5, 16 };
	player.scale = { 0.01f, 0.01f, 0.01f };

	player2.position = { 20, 5, 16 };
	player2.scale = { 0.01f, 0.01f, 0.01f };
	
	player.model = std::make_unique<Model>("");
	player2.model = std::make_unique<Model>("");
	
	cameraController.Update();
	cameraController.SyncControllerToCamera(camera);

}

// �I����
void SceneGame::Finalize()
{
	//�Q�[�W�I����
	
	//�J�����R���g���[���[�I����
	/*if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}*/
	//�v���C���[�I����
	/*if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}*/
	//�X�e�[�W�I����
	/*StageManager::Instance().Clear();*/
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	// �v���C���[�ړ�����
	{
		// ���͏���
		float axisX = 0.0f;
		float axisY = 0.0f;
		if (GetAsyncKeyState('W') & 0x8000) axisY += 1.0f;
		if (GetAsyncKeyState('S') & 0x8000) axisY -= 1.0f;
		if (GetAsyncKeyState('D') & 0x8000) axisX += 1.0f;
		if (GetAsyncKeyState('A') & 0x8000) axisX -= 1.0f;
		if (GetAsyncKeyState(VK_SPACE) & 0x01) player.velocity.y = jumpSpeed;

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
		player.velocity.y -= gravity * elapsedTime;

		// �ړ���
		float moveX = player.velocity.x * elapsedTime;
		float moveY = player.velocity.y * elapsedTime;
		float moveZ = player.velocity.z * elapsedTime;

		// �ڒn����
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

		// �ړ�
		player.position.x += moveX;
		player.position.y += moveY;
		player.position.z += moveZ;

	}
	////�X�e�[�W�X�V����
	//StageManager::Instance().Update(elapsedTime);
	// �v���C���[�s��X�V����
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(player.scale.x, player.scale.y, player.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(player.angle.x, player.angle.y, player.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(player.position.x, player.position.y, player.position.z);
		DirectX::XMStoreFloat4x4(&player.transform, S* R* T);
	}
	////�G�t�F�N�g�X�V����
	//EffectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render(float elapsedtime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	// ���f���`��
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &camera;
	modelRenderer->Render(rc, stage.transform, stage.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, player.transform, player.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, player2.transform, player2.model.get(), ShaderId::Lambert);
	
}
