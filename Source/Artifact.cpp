#include <optional>
#include "Artifact.h"

namespace Characters
{
	Artifact::Artifact(const std::string modelPath, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale)
		:
		state(CharacterState::None)
	{
		this->position = position;
		this->scale = scale;
		this->model = std::make_unique<Model>(modelPath.c_str());

		cameraController.controller.GetGamePad(&gamePad);
		cameraController.SyncCameraToCon();
	}

	void Artifact::Update(float elapsedTime)
	{
		DirectX::XMFLOAT3 target = position;
		target.y += 0.5f;
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
	}

	void Artifact::Guard(float elapsedTime)
	{
	}

	void Artifact::Push(float elapsedTime)
	{
	}

	void Artifact::Stan(float elapsedTime)
	{
	}

	void Artifact::InputHandler(float elapsedTime)
	{
		DirectX::XMFLOAT3 moveVec =  GetMoveVec();
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
		float speed = 5.0f * elapsedTime;

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

}