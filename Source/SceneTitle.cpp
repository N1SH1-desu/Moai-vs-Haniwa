#include "SceneTitle.h"
#include <algorithm>
#include "Graphics.h"
#include "GpuResourceUtils.h"
SceneTitle::SceneTitle()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	// テクスチャ
	{
		sprite = new Sprite(device,"Data/Sprite/Title.png");
		sprText = new Sprite(device, "Data/Font/font2.png");
	}
	//BGM,SE設定
	/*bgm = Audio::Instance().LoadAudioSource("Data/BGM/野良猫のワルツ.wav");
	bgm->Play(true);*/
}

SceneTitle::~SceneTitle()
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
	//BGM,SE再生終了
	/*bgm->Stop();*/
}

void SceneTitle::Update(float elapsedTime)
{
	
}

void SceneTitle::Render(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	//画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
	//テキスト描画
	sprText->textout(dc, "PUSH B", 240, 120, 110, 90, 150, 150, 30, 30, 0, 1, 0, 0, 0);

	//2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(sprite->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite->GetTextureHeight());
		

		//タイトルスプライト描画
		sprite->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
	}	
}
