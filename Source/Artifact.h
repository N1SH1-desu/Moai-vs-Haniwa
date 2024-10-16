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
		Artifact(const std::string modelPath, const DirectX::XMFLOAT3& position = { 0.0f, 0.0f, 0.0f}, const DirectX::XMFLOAT3& scale = { 1, 1, 1});
		virtual ~Artifact() = default;
		virtual void Update(float elapsedTime);
		virtual void Render(ID3D11DeviceContext* dc, const RenderState* renderState, ModelRenderer* modelRenderer, const Camera* camera);

		virtual void Attack(float elapsedTime);
		virtual void Guard(float elapsedTime);
		virtual void Push(float elapsedTime);
		virtual void Stan(float elapsedTime);

		virtual const std::string GetName() = 0;

		const Camera& GetCamera() const { return cameraController.camera; }
	protected:
		const DirectX::XMFLOAT4X4& UpdateTransform()
		{
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
			DirectX::XMMATRIX WorldTransform = S * R * T;
			DirectX::XMStoreFloat4x4(&transform, WorldTransform);

			return GetTransform();
		}
		const DirectX::XMFLOAT4X4& GetTransform()
		{
			return transform;
		}
	private:
		DirectX::XMFLOAT3 InputHandler();
		DirectX::XMFLOAT3 GetMoveVec();
		void Move(float elapsedTime, float vx, float vz);
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
		std::unique_ptr<Model>	model;

		CharacterState state;

		GamePad gamePad;
	};

}