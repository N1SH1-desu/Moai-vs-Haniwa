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
		// 移動入力処理
		float ax = gamePad.GetAxisLX();
		float ay = gamePad.GetAxisLY();

		// カメラの向きを計算
		const DirectX::XMFLOAT3& front = cameraController.camera.GetFront();
		const DirectX::XMFLOAT3& right = cameraController.camera.GetRight();
		float frontLengthXZ = sqrtf(front.x * front.x + front.z * front.z);
		float rightLengthXZ = sqrtf(right.x * right.x + right.z * right.z);
		float frontX = front.x / frontLengthXZ;
		float frontZ = front.z / frontLengthXZ;
		float rightX = right.x / rightLengthXZ;
		float rightZ = right.z / rightLengthXZ;

		// 移動ベクトルを求める
		float moveX = frontX * ay + rightX * ax;
		float moveZ = frontZ * ay + rightZ * ax;
		float moveLength = sqrtf(moveX * moveX + moveZ * moveZ);

		return { moveX, 0.0f, moveZ };
	}

	void Artifact::Move(float x, float z, float elapsedTime)
	{
		// 移動
		float speed = 5.0f * elapsedTime;
		position.x += x * speed;
		position.z += z * speed;

	}

	void Artifact::Turn(float x, float z, float elapsedTime)
	{
		float speed = 5.0f * elapsedTime;

		// 進行ベクトルがゼロベクトルの場合は処理する必要なし
		if (x == 0.0f && z == 0.0f)
			return;

		// 進行ベクトルを単位ベクトル化
		x /= sqrtf(powf(x, 2) + powf(z, 2));
		z /= sqrtf(powf(x, 2) + powf(z, 2));

		// 自身の回転値から前方向を求める
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);

		// 回転角を求めるため、２つの単位ベクトルの内積を計算する
		float dot = (frontX * x) + (frontZ * z);

		// 内積値は-1.0~1.0で表現されており、２つの単位ベクトルの角度が
		// 小さいほど1.0に近づくという性質を利用して回転速度を調整する
		// -1.0~1.0fから0.0~2.0にしている
		float rot = 1.0f - dot;

		// 左右判定を行うために２つの単位ベクトルの外積を計算する
		float cross = (frontZ * x) - (frontX * z);

		// 2Dの外積値が正の場合か負の場合によって左右判定が行える
		// 左右判定を行うことによって左右回転を選択する
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