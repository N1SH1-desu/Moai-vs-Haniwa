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
	}

	void Artifact::Update()
	{
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

	void Artifact::Attack()
	{
	}

	void Artifact::Guard()
	{
	}

	void Artifact::Push()
	{
	}

}