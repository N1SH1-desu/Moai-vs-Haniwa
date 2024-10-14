#include "Graphics.h"
#include "SceneTitle.h"

//#include"SceneGame.h"
//#include"SceneManager.h"
#include"Input.h"

//#include"SceneLoading.h"

// 初期化
void SceneTitle::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	
	// カメラ設定
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),    // 画角
		screenWidth / screenHeight,            // 画面アスペクト比
		0.1f,                                // ニアクリップ
		1000.0f                                // ファークリップ
	);
	camera.SetLookAt(
		{ 30, 20, 50 },        // 視点
		{ 20, 10, 20 },        // 注視点
		{ 0, 1, 0 }            // 上ベクトル
	);

	//camera.SetPerspectiveFov(0, 2, 0.1f, 1000);
	//camera.SetLookAt({0,0,0},{0,0,0},{0,0,0});
}
// 終了化
void SceneTitle::Finalize()
{
	// スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}
// 更新処理
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	// なにかボタンを押したらゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y
		;
	/*if (gamePad.GetButtonDown() & anyButton)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	}*/
}
// 描画処理
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; // RGBA(0.0〜1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
	// 2Dスプライト描画
	//{
	//	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	//	float screenHeight = static_cast<float>(graphics.GetScreenHeight());
	//	float textureWidth = static_cast<float>(sprite->GetTextureWidth());
	//	float textureHeight = static_cast<float>(sprite->GetTextureHeight());
	//	// タイトルスプライト描画 バグったら見る
	//	sprite->Render(dc,
	//		0, 0, screenWidth, screenHeight,
	//		0, 0, textureWidth, textureHeight,
	//		0,0,
	//		1, 1, 1, 1);
	//}
	
	// 3D描画
	{
		

		
	}

}