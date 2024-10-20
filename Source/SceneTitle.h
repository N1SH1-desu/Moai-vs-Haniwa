#pragma once
#include "Sprite.h"
#include "Scene.h"

class SceneTitle : public Scene
{
public:
	SceneTitle();
	~SceneTitle();

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(float elapsedTime) override;

	
	bool ruleSwitch;

private:
	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};
	Sprite* sprite = nullptr;
	Sprite* sprText=nullptr;
	Sprite* rule = nullptr;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				indexBuffer;

	D3D11_TEXTURE2D_DESC	textureDesc;

	//�I�[�f�B�I���\�[�X
	std::unique_ptr<AudioSource>bgm;
	std::unique_ptr<AudioSource>PushButtan;
	

};