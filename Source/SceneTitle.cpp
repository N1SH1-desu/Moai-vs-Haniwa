#include "SceneTitle.h"
#include <algorithm>
#include "Graphics.h"
#include "GpuResourceUtils.h"
SceneTitle::SceneTitle()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	// �e�N�X�`��
	{
		sprite = new Sprite(device,"Data/Sprite/Title.png");
	}
}

SceneTitle::~SceneTitle()
{
	
}

void SceneTitle::Update(float elapsedTime)
{
	
}

void SceneTitle::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	PrimitiveRenderer* primitiveRenderer = Graphics::Instance().GetPrimitiveRenderer();
	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();

	// �V�F�[�_�[�ݒ�
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);

	// �����_�[�X�e�[�g�ݒ�
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetInputLayout(inputLayout.Get());
	
	// ��ʃT�C�Y���e�N�X�`���T�C�Y�擾
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	float textureWidth = static_cast<float>(textureDesc.Width);
	float textureHeight = static_cast<float>(textureDesc.Height);


	sprite->Render(dc, 0, 0, 0, screenWidth, screenHeight, textureWidth, 0, 0, textureHeight,0,1,1,1,1);
}
