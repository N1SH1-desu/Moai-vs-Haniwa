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
	}

	void Artifact::Update(float elapsedTime)
	{
		DirectX::XMFLOAT3 target = position;
		target.y += 0.5f;
		cameraController.controller.SetFocus(target);

		cameraController.Update(elapsedTime);
		cameraController.SyncConToCamera();

		switch (state)
		{
		case CharacterState::None:
			DirectX::XMFLOAT3 moveVec{};
			moveVec = InputHandler();
			Move(elapsedTime, moveVec.x, moveVec.z);
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

	DirectX::XMFLOAT3 Artifact::InputHandler()
	{
		return GetMoveVec();
	}

	DirectX::XMFLOAT3 Artifact::GetMoveVec()
	{
		float ax = gamePad.GetAxisLX();
		float ay = gamePad.GetAxisLY();

		// �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
		const DirectX::XMFLOAT3& cameraRight = cameraController.camera.GetRight();
		const DirectX::XMFLOAT3& cameraFront = cameraController.camera.GetFront();

		// �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

		// �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
		float cameraRightX = cameraRight.x;
		float cameraRightZ = cameraRight.z;
		float cameraRightLength = sqrtf(powf(cameraRightX, 2) + powf(cameraRightZ, 2));
		if (cameraRightLength > 0.0f)
		{
			// �P�ʃx�N�g����
			cameraRightX /= cameraRightLength;
			cameraRightZ /= cameraRightLength;
		}

		// �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
		float cameraFrontX = cameraFront.x;
		float cameraFrontZ = cameraFront.z;
		float cameraFrontLength = sqrtf(powf(cameraFrontX, 2) + powf(cameraFrontZ, 2));
		if (cameraFrontLength > 0.0f)
		{
			// �P�ʃx�N�g����
			cameraFrontX /= cameraFrontLength;
			cameraFrontZ /= cameraFrontLength;
		}

		// �X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
		// �X�e�B�b�N�̐������͒l���J�����O�����񔽉f���A
		// �i�s�x�N�g�����v�Z����
		DirectX::XMFLOAT3 vec;
		vec.x = (ax * cameraRightX) + (ay * cameraFrontX);
		vec.z = (ax * cameraRightZ) + (ay * cameraFrontZ);
		// Y�������ɂ͈ړ����Ȃ�
		vec.y = 0.0f;

		return vec;
	}

	void Artifact::Move(float elapsedTime, float vx, float vz)
	{
		 float speed = 5.0f * elapsedTime;
		 position.x += vx * speed;
		 position.z *= vz * speed;
	}

}