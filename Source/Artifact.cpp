#include <optional>
#include "Artifact.h"
#include "Graphics.h"
#include "Collision.h"
#include "imgui.h"
#include "Easing.h"

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

		Turn(0.0f, 0.0f, elapsedTime);

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

	void Artifact::AttackMotion(float elapsedTime)
	{
		//Turn(0.0f, 0.0f, elapsedTime);

		float timeScale = 2.0f;

		attackMotionCurrentSeconds += elapsedTime * timeScale;
		if (attackMotionCurrentSeconds > 2.0f)
		{
			attackMotionCurrentSeconds = 0.0f;
			attackMotionAngle = 0.0f;
			state = CharacterState::None;
			return;
		}
		attackMotionAngle = AttackEasing(attackMotionCurrentSeconds / 2.0f) * DirectX::XM_2PI;

		DirectX::XMVECTOR base = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR camFront = DirectX::XMVector3Normalize(DirectX::XMVectorSet(cameraController.camera.GetFront().x, 0.0f, cameraController.camera.GetFront().z, 0.0f));

		DirectX::XMVECTOR rotateVec = DirectX::XMVector3Cross(base, camFront);
		attackQua = DirectX::XMQuaternionRotationAxis(rotateVec, attackMotionAngle);

		quaternion = DirectX::XMQuaternionMultiply(quaternion, attackQua);
	}

	void Artifact::Attack(float elapsedTime)
	{
		AttackMotion(elapsedTime);

		DirectX::XMFLOAT3 camFront = { cameraController.camera.GetFront().x, 0.0f, cameraController.camera.GetFront().z };
		DirectX::XMStoreFloat3(&camFront, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camFront)));
		DirectX::XMFLOAT3 setPos = { camFront.x * 1.5f, 0.0f, camFront.z * 1.5f };
		DirectX::XMFLOAT3 attackPosition = { position.x + setPos.x, position.y + 2.0f, position.z + setPos.z };

		DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&attackPosition), DirectX::XMLoadFloat3(&position));
		vec = DirectX::XMVector3Rotate(vec, attackQua);
		DirectX::XMFLOAT3 rotateVec;
		DirectX::XMStoreFloat3(&rotateVec, vec);

		attackPosition = { position.x + rotateVec.x, position.y + rotateVec.y, position.z + rotateVec.z };

		CollisionAttack(attackPosition);
		DrawAttackPrimitive(attackPosition);
	}

	void Artifact::GuardMotion(float elapsedTime)
	{
		//Turn(0.0f, 0.0f, elapsedTime);

		float timeScale = 2.0f;

		guardMotionCurrentSeconds += elapsedTime * timeScale;
		if (guardMotionCurrentSeconds > 2.0f)
		{
			guardMotionCurrentSeconds = 0.0f;
			guardMotionAngle = 0.0f;
			state = CharacterState::None;
			return;
		}
		guardMotionAngle = Easing(guardMotionCurrentSeconds / 2.0f) * DirectX::XM_2PI;

		DirectX::XMVECTOR guardQuate = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, guardMotionAngle, 0.0f);

		quaternion = DirectX::XMQuaternionMultiply(quaternion, guardQuate);
	}

	void Artifact::Guard(float elapsedTime)
	{
		GuardMotion(elapsedTime);
	}

	void Artifact::PushMotion(float elapsedTime)
	{
		Turn(0.0f, 0.0f, elapsedTime);

		float timeScale = 2.0f;

		pushMotionCurrentSeconds += elapsedTime * timeScale;
		if (pushMotionCurrentSeconds > 1.0f)
		{
			pushMotionCurrentSeconds = 0.0f;
			state = CharacterState::None;
			return;
		}
		float powerScale = PushEasing(pushMotionCurrentSeconds / 1.0f);

		DirectX::XMVECTOR camVec = DirectX::XMVector3Normalize(
			DirectX::XMVectorSet(cameraController.camera.GetFront().x, 0.0f, cameraController.camera.GetFront().z, 0.0f));

		camVec = DirectX::XMVectorScale(camVec, powerScale);
		DirectX::XMFLOAT3 vec; DirectX::XMStoreFloat3(&vec, camVec);

		position.x += vec.x * 0.05f;
		position.z += vec.z * 0.05f;
	}

	void Artifact::Push(float elapsedTime)
	{
		PushMotion(elapsedTime);
	}

	void Artifact::StanMotion(float elapsedTime)
	{
		const float stanMotionAll = 1.0f;
		const float stanMotionPer = stanMotionAll / 4.0f;
		stanMotionCurrentSeconds += elapsedTime;

		if (stanRotateCount > 5)
		{
			stanMotionCurrentSeconds = 0.0f;
			stanRotateCount = 0;
			state = CharacterState::None;
			return;
		}

		constexpr float angle = DirectX::XMConvertToRadians(-30);
		DirectX::XMVECTOR frontDiagonal = DirectX::XMQuaternionRotationAxis
		(
			DirectX::XMVector3Normalize(DirectX::XMVectorSet(cameraController.camera.GetRight().x, 0.0f, cameraController.camera.GetRight().z, 0.0f))
			, angle
		);
		DirectX::XMVECTOR rightDiagonal = DirectX::XMQuaternionRotationAxis
		(
			DirectX::XMVector3Normalize(DirectX::XMVectorSet(cameraController.camera.GetFront().x, 0.0f, cameraController.camera.GetFront().z, 0.0f)),
			angle
		);
		DirectX::XMVECTOR leftDiagonal = DirectX::XMQuaternionRotationAxis
		(
			DirectX::XMVector3Normalize(DirectX::XMVectorSet(cameraController.camera.GetFront().x, 0.0f, cameraController.camera.GetFront().z, 0.0f)),
			-angle
		);
		DirectX::XMVECTOR backDiagonal = DirectX::XMQuaternionRotationAxis
		(
			DirectX::XMVector3Normalize(DirectX::XMVectorSet(cameraController.camera.GetRight().x, 0.0f, cameraController.camera.GetRight().z, 0.0f))
			, -angle
		);


		if (stanMotionCurrentSeconds < stanMotionPer)
		{
			quaternion = DirectX::XMQuaternionMultiply(quaternion, DirectX::XMQuaternionSlerp(frontDiagonal, rightDiagonal, stanMotionCurrentSeconds));
		}
		else if (stanMotionCurrentSeconds > stanMotionPer && stanMotionCurrentSeconds < (stanMotionPer * 2.0f))
		{
			quaternion = DirectX::XMQuaternionMultiply(quaternion, DirectX::XMQuaternionSlerp(rightDiagonal, backDiagonal, stanMotionCurrentSeconds));
		}
		else if (stanMotionCurrentSeconds > (stanMotionPer * 2.0f) && stanMotionCurrentSeconds < (stanMotionPer * 3.0f))
		{
			quaternion = DirectX::XMQuaternionMultiply(quaternion, DirectX::XMQuaternionSlerp(backDiagonal, leftDiagonal, stanMotionCurrentSeconds));
		}
		else if (stanMotionCurrentSeconds > (stanMotionPer * 3.0f) && stanMotionCurrentSeconds < stanMotionAll)
		{
			quaternion = DirectX::XMQuaternionMultiply(quaternion, DirectX::XMQuaternionSlerp(leftDiagonal, frontDiagonal, stanMotionCurrentSeconds));
		}

		if (stanMotionCurrentSeconds >= stanMotionAll)
		{
			stanMotionCurrentSeconds = 0.0f;
			stanRotateCount++;
		}
	}

	void Artifact::Stan(float elapsedTime)
	{
		StanMotion(elapsedTime);
	}

	void Artifact::InputHandler(float elapsedTime)
	{
		DirectX::XMFLOAT3 moveVec =  GetMoveVec();
		Move(moveVec.x, moveVec.z, elapsedTime);
		//Turn(moveVec.x, moveVec.z, elapsedTime);

		GetAction();
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

		if (position.x >= 18.0f)
		{
			position.x = 18.0f;
		}
		if (position.x <= -18.0f)
		{
			position.x = -18.0f;
		}

		if (position.z >= 18.0f)
		{
			position.z = 18.0f;
		}
		if (position.z <= -18.0f)
		{
			position.z = -18.0f;
		}
	}

	void Artifact::Turn(float x, float z, float elapsedTime)
	{
#if 0
		float speed = 10.0f * elapsedTime;

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
#else
		DirectX::XMVECTOR camFront = DirectX::XMVector3Normalize(
			DirectX::XMVectorSet(cameraController.camera.GetFront().x, 0.0f, cameraController.camera.GetFront().z, 0.0f));
		DirectX::XMVECTOR Front = DirectX::XMLoadFloat3(&front);

		float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(camFront, Front));

		float angle = acosf(dot);

		DirectX::XMVECTOR cross = DirectX::XMVector3Cross(camFront, Front);
	
		quaternion = DirectX::XMQuaternionRotationAxis(cross, -angle);
#endif
	}

	void Artifact::GetAction()
	{
		if (gamePad.GetButtonDown() & GamePad::BTN_A)
		{
			state = CharacterState::Attack;
		}
		else if (gamePad.GetButtonDown() & GamePad::BTN_B)
		{
			state = CharacterState::Guard;
		}
		else if (gamePad.GetButtonDown() & GamePad::BTN_X)
		{
			state = CharacterState::Push;
		}
		else if (gamePad.GetButtonDown() & GamePad::BTN_Y)
		{
			state = CharacterState::Stan;
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

	void Artifact::CollisionAttack(DirectX::XMFLOAT3 attackPosition)
	{
		if (enemy == nullptr)
		{
			return;
		}

		DirectX::XMFLOAT3 outPosition{};
		if (Collision::IntersectSphereVsCylider(
			attackPosition, 1.0f,
			enemy->position, enemy->radius, enemy->height,
			outPosition
		))
		{

		}
	}

	void Artifact::CollisionPush()
	{
	}

	void Artifact::DrawDebugPrimitive(ShapeRenderer* shapeRenderer)
	{
		shapeRenderer->DrawCapsule(transform, 1.5f, 7.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	}

	void Artifact::DrawDebugGUI()
	{
		if (ImGui::Begin(u8"アーティファクト", nullptr))
		{
			ImGui::DragFloat3("CharacterPosition", &position.x, 0.01f);
		}
		ImGui::End();
	}

	void Artifact::DrawAttackPrimitive(DirectX::XMFLOAT3 attackPosition)
	{
		ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();

		shapeRenderer->DrawSphere(attackPosition, 1.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
	}

}