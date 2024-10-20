#include <imgui.h>
#include <ImGuizmo.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "Graphics.h"
#include "Collision.h"
#include "SceneGame.h"
#include "EffectManager.h"
#include "Moai.h"
#include "Haniwa.h"
// 初期化
 SceneGame::SceneGame()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	end = 0;
	GameTimer = 0;
	ring.position = { 0.0f, -5.0f, 0.0f };
	ring.scale = { 0.1f, 0.1f, 0.1f };
	ring.model = std::make_unique<Model>("Data/Model/Stage/stage.mdl");
	moai = std::make_unique<Characters::Moai>("Data/Model/Character/Moai.mdl", DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f));
	haniwa = std::make_unique<Characters::Haniwa>("Data/Model/Character/haniwa.mdl", DirectX::XMFLOAT3(4.0f, 0.0f, 10.0f), DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f));
	moai.get()->SetEnemy(haniwa.get());
	haniwa.get()->SetEnemy(moai.get());
	sprText = new Sprite(device, "Data/Font/font2.png");
	//ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/dead.efk");
	//BGM,SE設定
	bgm = Audio::Instance().LoadAudioSource("Data/BGM/ゲーム内音.wav");
	bgm->Play(true,0.3f);
	SEPunch = Audio::Instance().LoadAudioSource("Data/BGM/私は歌でぶん殴る！.wav");
	player1_death = false;
}

 SceneGame::~SceneGame()
 {
	 //BGM,SE再生終了
	 bgm->Stop();
 }

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	float timeScale = 1.0f;

	moai.get()->Update(elapsedTime);
	haniwa.get()->Update(elapsedTime);
	EffectManager::Instance().Update(elapsedTime);
	if (GetAsyncKeyState('Y') & 0x8000)
	{
		SEPunch->Play(false,1.0f);
	}
	
	/*if (player.death)
	{
		if (endBattle = 0)
		{
			endBattle = 1;
			GameTimer = 0;
		}
		player.position.x -= player2.position.x*0.1f;
		player.position.y +=0.4f;
		player.angle.z ++;
		if(GameTimer>180)end = 1;
	}
	if (player2.death)
	{
		if (endBattle = 0)
		{
			endBattle = 1;
			GameTimer = 0;
		}
		player2.position.x -= player.position.x * 0.1f;
		player2.position.y += 0.4f;
		player2.angle.z++;
		if(GameTimer > 180)end = 2;
	}*/
	GameTimer++;
}

// 描画処理
void SceneGame::Render(float elapsedtime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	PrimitiveRenderer* primitiveRenderer = Graphics::Instance().GetPrimitiveRenderer();
	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	// モデル描画
	Graphics::Instance().SetRenderTargets(1);
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &moai.get()->GetCamera();
	modelRenderer->Render(rc, ring.transform, ring.model.get(), ShaderId::Lambert);
	moai.get()->Render(dc, renderState, modelRenderer, &moai.get()->GetCamera());
	haniwa.get()->Render(dc, renderState, modelRenderer, &moai.get()->GetCamera());

	moai.get()->DrawDebugPrimitive(shapeRenderer);
	haniwa.get()->DrawDebugPrimitive(shapeRenderer);
	shapeRenderer->Render(dc, moai.get()->GetCamera().GetView(), moai.get()->GetCamera().GetProjection());

	EffectManager::Instance().Render(rc.camera->GetView(), rc.camera->GetProjection());

	Graphics::Instance().SetRenderTargets(2);
	rc.camera = &haniwa.get()->GetCamera();
	modelRenderer->Render(rc, ring.transform, ring.model.get(), ShaderId::Lambert);
	moai.get()->Render(dc, renderState, modelRenderer, &haniwa.get()->GetCamera());
	//haniwa.get()->Render(dc, renderState, modelRenderer, &haniwa.get()->GetCamera());
	//modelRenderer->Render(rc, character.transform, character.model.get(), ShaderId::Lambert);
	moai.get()->Render(dc, renderState, modelRenderer, &haniwa.get()->GetCamera());
	haniwa.get()->Render(dc, renderState, modelRenderer, &haniwa.get()->GetCamera());


	//shapeRenderer->Render(dc, cameraController.GetCamera(CAMERA_NAME::HANIWA).GetView(), cameraController.GetCamera(CAMERA_NAME::HANIWA).GetProjection());
	EffectManager::Instance().Render(rc.camera->GetView(), rc.camera->GetProjection());
}
