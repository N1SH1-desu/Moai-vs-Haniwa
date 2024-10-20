#include "SceneResult.h"
#include <algorithm>
#include "Graphics.h"
#include "GpuResourceUtils.h"
#include "SceneGame.h"
bool player1_death;
SceneResult::SceneResult()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	// テクスチャ
	{
		moai = new Sprite(device, "Data/Sprite/moai_win.png");
		haniwa = new Sprite(device, "Data/Sprite/haniwa_win.png");
		sprText = new Sprite(device, "Data/Font/font2.png");
	}
}

SceneResult::~SceneResult()
{
	if (moai != false)
	{
		delete moai;
		moai = nullptr;
	}
	if (haniwa != false)
	{
		delete haniwa;
		haniwa = nullptr;
	}
	if (sprText != false)
	{
		delete sprText;
		sprText = nullptr;
	}
}

void SceneResult::Update(float elapsedTime)
{
}

void SceneResult::Render(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());
	float textureWidth = static_cast<float>(moai->GetTextureWidth());
	float textureHeight = static_cast<float>(moai->GetTextureHeight());
	float textureWidth2 = static_cast<float>(haniwa->GetTextureWidth());
	float textureHeight2 = static_cast<float>(haniwa->GetTextureHeight());

	//画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
	//sprText->textout(dc, "PUSH B", 420, 650, 110, 90, 150, 150, 30, 30, 0, 1, 0, 0, 0);
	if (player1_death)//プレイヤー1が死んだとき
	{
		//sprText->textout(dc, "Haniwa", 240, 120, 110, 90, 150, 150, 30, 30, 0, 1, 0, 0, 0);
		//sprText->textout(dc, "Win", 240, 300, 110, 90, 150, 150, 30, 30, 0, 1, 0, 0, 0);
		haniwa->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth2, textureHeight2,
			0,
			1, 1, 1, 1);
	}
	if (!player1_death)
	{

		//sprText->textout(dc, "Moai", 240, 120, 110, 90, 150, 150, 30, 30, 0, 1, 0, 0, 0);
		//sprText->textout(dc, "Win", 240, 300, 110, 90, 150, 150, 30, 30, 0, 1, 0, 0, 0);
		moai->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
	}
}