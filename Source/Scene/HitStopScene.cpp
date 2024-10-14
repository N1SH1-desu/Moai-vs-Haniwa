#include <imgui.h>
#include <ImGuizmo.h>
#include <DirectXCollision.h>
#include "Graphics.h"
#include "Collision.h"
#include "Scene/HitStopScene.h"
#include "Moai.h"
#include "Haniwa.h"

// コンストラクタ
HitStopScene::HitStopScene()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	moai = std::make_unique<Characters::Moai>("Data/Model/Mr.Incredible/Mr.Incredible.mdl", DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.01, 0.01, 0.01));
	haniwa = std::make_unique<Characters::Haniwa>("Data/Model/RPG-Character/2Hand-Sword.mdl", DirectX::XMFLOAT3(0, 1, 1.5f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
}

// 更新処理
void HitStopScene::Update(float elapsedTime)
{
	cameraController.Update();

	float timeScale = 1.0f;

	moai.get()->Update(elapsedTime);
	haniwa.get()->Update(elapsedTime);
}

// 描画処理
void HitStopScene::Render(float elapsedTime)
{
	using CAMERA_NAME = DoubleCameraController::CAMERA_IDENTIFIER;

	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	PrimitiveRenderer* primitiveRenderer = Graphics::Instance().GetPrimitiveRenderer();
	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	// モデル描画
	Graphics::Instance().SetRenderTargets(0);
	//RenderContext rc;
	//rc.deviceContext = dc;
	//rc.renderState = renderState;
	//rc.camera = &cameraController.GetCamera(CAMERA_NAME::MOAI);
	//modelRenderer->Render(rc, character.transform, character.model.get(), ShaderId::Lambert);
	//modelRenderer->Render(rc, weapon.transform, weapon.model.get(), ShaderId::Lambert);
	moai.get()->Render(dc, renderState, modelRenderer, &cameraController.GetCamera(CAMERA_NAME::MOAI));
	haniwa.get()->Render(dc, renderState, modelRenderer, &cameraController.GetCamera(CAMERA_NAME::MOAI));

	//DirectX::XMFLOAT3 p;
	//p.x = weapon.transform._41 + weapon.transform._31 * weaponHitOffset;
	//p.y = weapon.transform._42 + weapon.transform._32 * weaponHitOffset;
	//p.z = weapon.transform._43 + weapon.transform._33 * weaponHitOffset;
	//shapeRenderer->DrawSphere(p, weaponHitRadius, { 1,1,1,1 });

	//p.x = character.position.x;
	//p.y = character.position.y + characterHitOffset;
	//p.z = character.position.z;
	//shapeRenderer->DrawSphere(p, characterHitRadius, { 1,1,1,1 });
	//shapeRenderer->Render(dc, cameraController.GetCamera(CAMERA_NAME::MOAI).GetView(), cameraController.GetCamera(CAMERA_NAME::MOAI).GetProjection());

	//// 軸描画
	//dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	//dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::NoTestNoWrite), 0);
	//dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	//primitiveRenderer->DrawAxis(weapon.transform, { 1,1,1,1 });
	//primitiveRenderer->Render(dc, cameraController.GetCamera(CAMERA_NAME::MOAI).GetView(), cameraController.GetCamera(CAMERA_NAME::MOAI).GetProjection(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//// グリッド描画
	//dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	//dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	//dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	//primitiveRenderer->DrawGrid(20, 1);
	//primitiveRenderer->Render(dc, cameraController.GetCamera(CAMERA_NAME::MOAI).GetView(), cameraController.GetCamera(CAMERA_NAME::MOAI).GetProjection(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);



	Graphics::Instance().SetRenderTargets(1);
	moai.get()->Render(dc, renderState, modelRenderer, &cameraController.GetCamera(CAMERA_NAME::HANIWA));
	haniwa.get()->Render(dc, renderState, modelRenderer, &cameraController.GetCamera(CAMERA_NAME::HANIWA));
	//rc.camera = &cameraController.GetCamera(CAMERA_NAME::HANIWA);
	//modelRenderer->Render(rc, character.transform, character.model.get(), ShaderId::Lambert);
	//modelRenderer->Render(rc, weapon.transform, weapon.model.get(), ShaderId::Lambert);

	//shapeRenderer->Render(dc, cameraController.GetCamera(CAMERA_NAME::HANIWA).GetView(), cameraController.GetCamera(CAMERA_NAME::HANIWA).GetProjection());

	//// 軸描画
	//dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	//dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::NoTestNoWrite), 0);
	//dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	//primitiveRenderer->DrawAxis(weapon.transform, { 1,1,1,1 });
	//primitiveRenderer->Render(dc, cameraController.GetCamera(CAMERA_NAME::HANIWA).GetView(), cameraController.GetCamera(CAMERA_NAME::HANIWA).GetProjection(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//// グリッド描画
	//dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	//dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	//dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	//primitiveRenderer->DrawGrid(20, 1);
	//primitiveRenderer->Render(dc, cameraController.GetCamera(CAMERA_NAME::HANIWA).GetView(), cameraController.GetCamera(CAMERA_NAME::HANIWA).GetProjection(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

// GUI描画処理
void HitStopScene::DrawGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();

	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 180), ImGuiCond_Once);

	if (ImGui::Begin(u8"ヒットストップ", nullptr, ImGuiWindowFlags_NoNavInputs))
	{
		//ImGui::DragFloat3("CharacterPosition", &character.position.x, 0.01f);
		//ImGui::DragFloat3("WeaponPosition", &weapon.position.x, 0.01f);
		//ImGui::DragFloat3("WeaponAngle", &weapon.angle.x, 0.01f);

		ImGui::DragFloat("weaonHitOffset", &weaponHitOffset, 0.01f);
		ImGui::DragFloat("weaonHitRadius", &weaponHitRadius, 0.01f);
	}
	ImGui::End();
}
