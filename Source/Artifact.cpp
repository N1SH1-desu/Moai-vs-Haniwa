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

		// カメラ方向とスティックの入力値によって進行方向を計算する
		const DirectX::XMFLOAT3& cameraRight = cameraController.camera.GetRight();
		const DirectX::XMFLOAT3& cameraFront = cameraController.camera.GetFront();

		// 移動ベクトルはXZ平面に水平なベクトルになるようにする

		// カメラ右方向ベクトルをXZ単位ベクトルに変換
		float cameraRightX = cameraRight.x;
		float cameraRightZ = cameraRight.z;
		float cameraRightLength = sqrtf(powf(cameraRightX, 2) + powf(cameraRightZ, 2));
		if (cameraRightLength > 0.0f)
		{
			// 単位ベクトル化
			cameraRightX /= cameraRightLength;
			cameraRightZ /= cameraRightLength;
		}

		// カメラ前方向ベクトルをXZ単位ベクトルに変換
		float cameraFrontX = cameraFront.x;
		float cameraFrontZ = cameraFront.z;
		float cameraFrontLength = sqrtf(powf(cameraFrontX, 2) + powf(cameraFrontZ, 2));
		if (cameraFrontLength > 0.0f)
		{
			// 単位ベクトル化
			cameraFrontX /= cameraFrontLength;
			cameraFrontZ /= cameraFrontLength;
		}

		// スティックの水平入力値をカメラ右方向に反映し、
		// スティックの垂直入力値をカメラ前方向二反映し、
		// 進行ベクトルを計算する
		DirectX::XMFLOAT3 vec;
		vec.x = (ax * cameraRightX) + (ay * cameraFrontX);
		vec.z = (ax * cameraRightZ) + (ay * cameraFrontZ);
		// Y軸方向には移動しない
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