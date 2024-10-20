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
		sprite = new Sprite(device, "Data/Sprite/Title.png");
		sprText = new Sprite(device, "Data/Font/font2.png");
	}
}

SceneResult::~SceneResult()
{
	if (sprite != false)
	{
		delete sprite;
		sprite = nullptr;
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
	graphics.SetRenderTargets(0);
	//画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
	//sprText->textout(dc, "PUSH B", 240, 600, 110, 90, 150, 150, 30, 30, 0, 1, 0, 0, 0);
	if (player1_death)//プレイヤー1が死んだとき
	{
		sprText->textout(dc, "Player2", 240, 120, 110, 90, 150, 150, 30, 30, 0, 1, 0, 0, 0);
		sprText->textout(dc, "Win", 240, 300, 110, 90, 150, 150, 30, 30, 0, 1, 0, 0, 0);
	}
	if (!player1_death)
	{
		sprText->textout(dc, "Player1", 240, 120, 110, 90, 150, 150, 30, 30, 0, 1, 0, 0, 0);
		sprText->textout(dc, "Win", 240, 300, 110, 90, 150, 150, 30, 30, 0, 1, 0, 0, 0);
	}
}
