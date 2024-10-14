#include "Graphics.h"
#include "EffectManager.h"

//初期化

void EffectManager::initialize()
{
	Graphics& graphics = Graphics::Instance();

	//Effekseerレンダラ作成
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(
		graphics.GetDevice(), graphics.GetDeviceContext(), 2048);

	//Effekseerマネージャー作成
	effekseerManager = Effekseer::Manager::Create(2048);

	//Effekseerレンダラの各種設定(特別なカスタマイズをしなければ以下のままでOK)
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());

	//Effekseer内でのローダー設定
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

void EffectManager::Finalize()
{
	//スマートポインターによって削除されるので何もしない
}

void EffectManager::Update(float elapsedTime)
{
	//エフェクト更新処理
	effekseerManager->Update(elapsedTime * 60.0f);
}

void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	//ビュー＆プロジェクション行列をEffekseerレンダラに設定
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const
		Effekseer::Matrix44*>(&projection));

	//Effekseer描画開始
	effekseerRenderer->BeginRendering();

	//Effekseer描画実行
	//マネージャー単位で描画するので描画順を制御する場合は
	//マネージャーを複数作成し、Draw()関数を実行する順序で制御
	effekseerManager->Draw();

	//Effekseer描画開始
	effekseerRenderer->EndRendering();
}
