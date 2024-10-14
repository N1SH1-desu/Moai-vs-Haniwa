#include "Artifact.h"

namespace Characters
{
	Artifact::Artifact(const std::string modelPath, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale)
	{
		this->position = position;
		this->scale = scale;
		this->model = std::make_unique<Model>(modelPath.c_str());
	}

	void Artifact::Update(float elapsedTime)
	{
		float timeScale = 1.0f;

		UpdateTransform();

		CharactersMotionCurrentSeconds += elapsedTime * timeScale;
		if (CharactersMotionCurrentSeconds > CharactersMotionSecondsLength)
		{
			CharactersMotionCurrentSeconds = 0.0f;
		}

		if (GetAsyncKeyState(0x51) & 0x8000)
		{
			angle.y = Easing(CharactersMotionCurrentSeconds / CharactersMotionSecondsLength) * DirectX::XM_2PI;
		}
	}

	void Artifact::Render(ID3D11DeviceContext* dc, const RenderState* renderState, ModelRenderer* modelRenderer, const Camera* camera)
	{
		RenderContext rc;
		rc.deviceContext = dc;
		rc.renderState = renderState;
		rc.camera = camera;
		modelRenderer->Render(rc, transform, model.get(), ShaderId::Lambert);
	}

	
}