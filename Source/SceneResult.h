#pragma once
#include "Sprite.h"
#include "Scene.h"

class SceneResult : public Scene
{
public:
	SceneResult();
	~SceneResult();

	// çXêVèàóù
	void Update(float elapsedTime) override;

	// ï`âÊèàóù
	void Render(float elapsedTime) override;




private:
	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};
	Sprite* moai = nullptr;
	Sprite* sprText = nullptr;
	Sprite* haniwa = nullptr;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				indexBuffer;

	D3D11_TEXTURE2D_DESC	textureDesc;



};