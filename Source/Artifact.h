#pragma once
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <Model.h>
#include "ModelRenderer.h"
#include "Camera.h"
#include "RenderState.h"

namespace Characters
{

	// モアイとハニワの基底クラス
	class Artifact
	{
	public:
		Artifact() = delete;
		Artifact(const Artifact&) = delete;
		Artifact& operator=(const Artifact&) = delete;
		Artifact(const std::string modelPath, const DirectX::XMFLOAT3& position = { 0.0f, 0.0f, 0.0f}, const DirectX::XMFLOAT3& scale = { 1, 1, 1});
		virtual ~Artifact() = default;
		virtual void Update();
		virtual void Render(ID3D11DeviceContext* dc, const RenderState* renderState, ModelRenderer* modelRenderer, const Camera* camera);

		virtual void Attack(float elapsedTime);
		virtual void Push(float elapsedTime);

	private:
		float AttackEasing(float x)
		{

			const float c1 = 1.70158;
			const float c2 = c1 * 1.525;

			return x < 0.5
				? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
				: (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
			
		}

		float PushEasing(float x)
		{
			const float c1 = 1.70158;
			const float c2 = c1 * 1.525;

			return x < 0.5
				? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
				: (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
		}

		float								CharactersMotionCurrentSeconds = 0.0f;
		float								CharactersMotionSecondsLength = 1.0f;

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
	protected:
		bool					onGround = false;
		DirectX::XMFLOAT3		velocity = { 0, 0, 0 };
		DirectX::XMFLOAT3		position = { 0, 0, 0 };
		DirectX::XMFLOAT3		angle = { 0, 0, 0 };
		DirectX::XMFLOAT3		scale = { 1, 1, 1 };
		DirectX::XMFLOAT4X4		transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		std::unique_ptr<Model>	model;
	};

}