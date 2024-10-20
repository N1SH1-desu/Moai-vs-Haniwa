#pragma once
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <Model.h>
#include "ModelRenderer.h"
#include "Camera.h"
#include "FreeCameraController.h"
#include "RenderState.h"
#include "Input/GamePad.h"
#include "ShapeRenderer.h"
#include "Effect.h"
#include<vector>
#include"Audio.h"
#include"AudioSource.h"

namespace Characters
{
	enum  class CharacterState
	{
		None,
		Stan,
		Attack,
		Guard,
		Push,
	};

	// モアイとハニワの基底クラス
	class Artifact
	{
	public:
		Artifact() = delete;
		Artifact(const Artifact&) = delete;
		Artifact& operator=(const Artifact&) = delete;
		Artifact(const std::string modelPath, const DirectX::XMFLOAT3& position = { 0.0f, 0.0f, 0.0f }, const DirectX::XMFLOAT3& scale = { 1, 1, 1 }, Artifact* enemy = nullptr);
		virtual ~Artifact() = default;
		virtual void Update(float elapsedTime);
		virtual void Render(ID3D11DeviceContext* dc, const RenderState* renderState, ModelRenderer* modelRenderer, const Camera* camera);
		void DrawDebugPrimitive(ShapeRenderer* shapeRenderer);

		void SetEnemy(Artifact* enem) { enemy = enem; }

		
		virtual const std::string GetName() = 0;

		const Camera& GetCamera() const { return cameraController.camera; }

		const DirectX::XMFLOAT3 GetPos()const { return position; }
		
	protected:

		virtual void AttackMotion(float elapsedTime);
		virtual void GuardMotion(float elapsedTime);
		virtual void PushMotion(float elapsedTime);
		virtual void StanMotion(float elapsedTime);

		const DirectX::XMFLOAT4X4& UpdateTransform()
		{
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(quaternion);
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
			DirectX::XMMATRIX WorldTransform = S * R * T;
			DirectX::XMStoreFloat4x4(&transform, WorldTransform);
			model->UpdateTransform();

			return GetTransform();
		}
		const DirectX::XMFLOAT4X4& GetTransform()
		{
			return transform;
		}
	private:
		void InputHandler(float elapsedTime);
		DirectX::XMFLOAT3 GetMoveVec();
		void Move(float x, float z, float elapsedTime);
		void Turn(float x, float z, float elapsedTime);

		void Attack(float elapsedTime);
		void Guard(float elapsedTime);
		void Push(float elapsedTime);
		void Stan(float elapsedTime);
		void GetAction();

		void lowHp(DirectX::XMFLOAT3 position);
		void CollisionPlayerVsEnemies();
		void CollisionAttack(DirectX::XMFLOAT3 attackPosition);
		void CollisionPush(DirectX::XMFLOAT3 pushPosition);
		void DrawDebugGUI();
		void DrawAttackPrimitive(DirectX::XMFLOAT3 attackPosition);
		void death();
		void Artifact::DrawPushPrimitive(DirectX::XMFLOAT3 pushPosition);

	protected:
		struct CameraPair
		{
			Camera camera;
			FreeCameraController controller;

			void SyncCameraToCon() { controller.SyncCameraToController(camera); }
			void SyncConToCamera() { controller.SyncControllerToCamera(camera); }

			void Update(float elapsedTime)
			{
				controller.Update(elapsedTime);
				controller.SyncControllerToCamera(camera);
			}
		};
		CameraPair cameraController;
		bool					onGround = false;
		DirectX::XMFLOAT3		velocity = { 0, 0, 0 };
		DirectX::XMFLOAT3		position = { 0, 0, 0 };
		DirectX::XMFLOAT3		angle = { 0, 0, 0 };
		DirectX::XMFLOAT3		scale = { 1, 1, 1 };
		DirectX::XMFLOAT4X4		transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		DirectX::XMFLOAT3       front = { 0, 0, 1 };
		DirectX::XMVECTOR       quaternion;
		float					radius = 1.5f;
		float					height = 10.0f;
		float					speed = 4.0f;
		int						timer = 0;
		std::unique_ptr<Model>	model;
		int health = 5;
		bool onHit = false;
		float noHitTimer = 0.0f;
		bool hitFlag = false;
		

		float attackMotionCurrentSeconds = 0.0f;
		float attackMotionAngle = 0.0f;
		DirectX::XMVECTOR attackQua;
		float guardMotionCurrentSeconds = 0.0f;
		float guardMotionAngle = 0.0f;
		float pushMotionCurrentSeconds = 0.0f;
		float stanMotionCurrentSeconds = 0.0f;
		int  stanRotateCount = 0;
		

		CharacterState state;
		GamePad gamePad;
		Artifact* enemy;
		Effect* deadEfk = nullptr;
		Effect* GuardEfk = nullptr;
		Effect* punch = nullptr;
		Effect* kemuri = nullptr;
		std::unique_ptr<AudioSource> SEKO;
		std::unique_ptr<AudioSource> SEGuard;
		std::unique_ptr<AudioSource> SEPush;
		std::unique_ptr<AudioSource> SEPunch;
	};

}