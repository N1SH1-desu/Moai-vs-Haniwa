#include <optional>
#include "Artifact.h"
#include "Graphics.h"
#include "Collision.h"
#include "imgui.h"

namespace Characters
{
	Artifact::Artifact(const std::string modelPath, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, Artifact* enemey)
		:
		state(CharacterState::None),
		enemy(enemey)
	{
		this->position = position;
		this->scale = scale;
		this->model = std::make_unique<Model>(modelPath.c_str());

		cameraController.controller.GetGamePad(&gamePad);
		cameraController.SyncCameraToCon();
		deadEfk = new Effect("Data/Effect/dead.efk");
		GuardEfk = new Effect("Data/Effect/guardSuccess.efk");
		punch = new Effect("Data/Effect/punch.efk");
		kemuri = new Effect("Data/Effect/kemuri.efk");
	}

	void Artifact::Update(float elapsedTime)
	{
		DirectX::XMFLOAT3 target = position;
		target.y += 0.5f;
		DirectX::XMVECTOR normalRightVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&cameraController.camera.GetRight()));
		DirectX::XMFLOAT3 rightVec;
		DirectX::XMStoreFloat3(&rightVec, normalRightVec);
		target.x += rightVec.x;
		target.y += rightVec.y;
		target.z += rightVec.z;
		cameraController.controller.SetFocus(target);

		gamePad.Update();
		cameraController.Update(elapsedTime);

		switch (state)
		{
		case CharacterState::None:
			InputHandler(elapsedTime);
			break;
		case CharacterState::Stan:
			Stan(elapsedTime);
			break;
		case CharacterState::Attack:
			Attack(elapsedTime);
			break;
		case CharacterState::Guard:
			Guard(elapsedTime);
			break;
		case CharacterState::Push:
			Push(elapsedTime);
			break;
		}

		if (hp <= 2)timer++;
		if (hp <= 0)death();
		lowHp(elapsedTime);
		CollisionPlayerVsEnemies();
		DrawDebugGUI();
		UpdateTransform();
	}

	void Artifact::Render(ID3D11DeviceContext* dc, const RenderState* renderState, ModelRenderer* modelRenderer, const Camera* camera)
	{
		RenderContext rc;
		rc.deviceContext = dc;
		rc.renderState = renderState;
		rc.camera = camera;
		modelRenderer->Render(rc, transform, model.get(), ShaderId::Lambert);
	}

	void Artifact::Attack(float elapsedTime)
	{
		//if (CollisionPlayerVsEnemies())
		{
			enemy->hp--;
			punch->Play(enemy->position);

		}
	}

	void Artifact::Guard(float elapsedTime)
	{
		//if (CollisionPlayerVsEnemies())
		{
			GuardEfk->Play(position);
		}
	}

	void Artifact::Push(float elapsedTime)
	{
	}

	void Artifact::Stan(float elapsedTime)
	{
	}

	void Artifact::InputHandler(float elapsedTime)
	{
		DirectX::XMFLOAT3 moveVec = GetMoveVec();
		Move(moveVec.x, moveVec.z, elapsedTime);
		Turn(moveVec.x, moveVec.z, elapsedTime);
	}

	DirectX::XMFLOAT3 Artifact::GetMoveVec()
	{
		// �ړ����͏���
		float ax = gamePad.GetAxisLX();
		float ay = gamePad.GetAxisLY();

		// �J�����̌������v�Z
		const DirectX::XMFLOAT3& front = cameraController.camera.GetFront();
		const DirectX::XMFLOAT3& right = cameraController.camera.GetRight();
		float frontLengthXZ = sqrtf(front.x * front.x + front.z * front.z);
		float rightLengthXZ = sqrtf(right.x * right.x + right.z * right.z);
		float frontX = front.x / frontLengthXZ;
		float frontZ = front.z / frontLengthXZ;
		float rightX = right.x / rightLengthXZ;
		float rightZ = right.z / rightLengthXZ;

		// �ړ��x�N�g�������߂�
		float moveX = frontX * ay + rightX * ax;
		float moveZ = frontZ * ay + rightZ * ax;
		float moveLength = sqrtf(moveX * moveX + moveZ * moveZ);

		return { moveX, 0.0f, moveZ };
	}

	void Artifact::Move(float x, float z, float elapsedTime)
	{
		// �ړ�
		float speed = 5.0f * elapsedTime;
		position.x += x * speed;
		position.z += z * speed;

	}

	void Artifact::Turn(float x, float z, float elapsedTime)
	{
		float speed = 10.0f * elapsedTime;

		// �i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
		if (x == 0.0f && z == 0.0f)
			return;

		// �i�s�x�N�g����P�ʃx�N�g����
		x /= sqrtf(powf(x, 2) + powf(z, 2));
		z /= sqrtf(powf(x, 2) + powf(z, 2));

		// ���g�̉�]�l����O���������߂�
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);

		// ��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
		float dot = (frontX * x) + (frontZ * z);

		// ���ϒl��-1.0~1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
		// �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
		// -1.0~1.0f����0.0~2.0�ɂ��Ă���
		float rot = 1.0f - dot;

		// ���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
		float cross = (frontZ * x) - (frontX * z);

		// 2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
		// ���E������s�����Ƃɂ���č��E��]��I������
		if (cross < 0.0f)
		{
			angle.y += -speed * rot;
		}
		else
		{
			angle.y += speed * rot;
		}
	}

	void Artifact::lowHp(float elapsedTime)
	{
		if (hp <= 2&&timer==0)
		{
			kemuri->Play(position);
		}
	}

	void Artifact::CollisionPlayerVsEnemies()
	{
		if (enemy == nullptr)
		{
			return;
		}

		DirectX::XMFLOAT3 outPosition{};
		if (Collision::IntersectCylinderVsCylinder(
			position, radius, height,
			enemy->position, enemy->radius, enemy->height,
			outPosition
		))
		{

			enemy->position = { outPosition.x, outPosition.y, outPosition.z };

		}
	}

	void Artifact::DrawDebugPrimitive(ShapeRenderer* shapeRenderer)
	{
		shapeRenderer->DrawCapsule(transform, 1.5f, 7.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	}

	void Artifact::DrawDebugGUI()
	{
		if (ImGui::Begin(u8"�A�[�e�B�t�@�N�g", nullptr))
		{
			ImGui::DragFloat3("CharacterPosition", &position.x, 0.01f);
		}
		ImGui::End();
	}

	void Artifact::death()
	{
		DirectX::XMFLOAT3 vec;
		vec.x =  enemy->position.x - position.x;
		vec.z =  enemy->position.z - position.z;
		DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vec));
		position.x = -vec.x * speed;
		position.y += 0.4f;
		position.z = -vec.z * speed;

			deadEfk->Play(position);
	}

}