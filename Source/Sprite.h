#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <sstream>

// �X�v���C�g
class Sprite
{
public:
	Sprite(ID3D11Device* device);
	Sprite(ID3D11Device* device, const char* filename);

	// �e�N�X�`�����擾
	int GetTextureWidth() const { return textureWidth; }

	// �e�N�X�`�������擾
	int GetTextureHeight() const { return textureHeight; }
	// ���_�f�[�^
	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

	// �`����s
	void Render(ID3D11DeviceContext* dc,
		float dx, float dy,					// ����ʒu
		float dz,							// ���s
		float dw, float dh,					// ���A����
		float sx, float sy,					// �摜�؂蔲���ʒu
		float sw, float sh,					// �摜�؂蔲���T�C�Y
		float angle,						// �p�x
		float r, float g, float b, float a	// �F
	) const;

	// �`����s�i�e�N�X�`���؂蔲���w��Ȃ��j
	void Render(ID3D11DeviceContext* dc,
		float dx, float dy,					// ����ʒu
		float dz,							// ���s
		float dw, float dh,					// ���A����
		float angle,						// �p�x
		float r, float g, float b, float a	// �F
	) const;

	void Render(ID3D11DeviceContext* dc,
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a) const;

	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle/*degree*/, float sx, float sy, float sw, float sh);
	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);
	void textout(ID3D11DeviceContext* dc,
		std::string s,
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a);
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
	D3D11_TEXTURE2D_DESC texture2d_desc;

	float textureWidth = 0;
	float textureHeight = 0;
};
