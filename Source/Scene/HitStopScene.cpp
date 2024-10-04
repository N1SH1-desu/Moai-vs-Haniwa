#include <imgui.h>
#include <ImGuizmo.h>
#include <DirectXCollision.h>
#include "Graphics.h"
#include "Collision.h"
#include "Scene/HitStopScene.h"

// コンストラクタ
HitStopScene::HitStopScene()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	cameraMoai.SetLookAt(
		{ 4, 2, 2 },		// 視点
		{ 0, 1.5f, 1 },		// 注視点
		{ 0, 1, 0 }			// 上ベクトル
	);
	cameraHaniwa.SetLookAt(
		{ 4, 2, -2 },
		{ 0, 1.5f, 1 },
		{ 0, 1, 0 }
	);
	cameraMoaiController.SyncCameraToController(cameraMoai);
	cameraHaniwaController.SyncCameraToController(cameraHaniwa);

	character.position = { 0, 0, 0 };
	character.scale = { 0.01f, 0.01f, 0.01f };
	character.model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	weapon.position = { 0, 1, 1.5f };
	weapon.angle = { 0, 0, 0 };
	weapon.scale = { 1.0f, 1.0f, 1.0f };
	weapon.model = std::make_unique<Model>("Data/Model/RPG-Character/2Hand-Sword.mdl");
}

// 更新処理
void HitStopScene::Update(float elapsedTime)
{
	// カメラ更新処理
	cameraMoaiController.Update();
	cameraMoaiController.SyncControllerToCamera(cameraMoai);
	cameraHaniwaController.Update();
	cameraHaniwaController.SyncControllerToCamera(cameraHaniwa);


	float timeScale = 1.0f;

	// 剣の動き
	weaponMotionCurrentSeconds += elapsedTime * timeScale;
	if (weaponMotionCurrentSeconds > weaponMotionSecondsLength)
	{
		weaponMotionCurrentSeconds = 0.0f;
	}
	weapon.angle.y = Easing(weaponMotionCurrentSeconds / weaponMotionSecondsLength) * DirectX::XM_2PI;

	// キャラクター行列更新処理
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(character.scale.x, character.scale.y, character.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(character.angle.x, character.angle.y, character.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(character.position.x, character.position.y, character.position.z);
		DirectX::XMStoreFloat4x4(&character.transform, S * R * T);
	}

	// 武器行列更新処理
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(weapon.scale.x, weapon.scale.y, weapon.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(weapon.angle.x, weapon.angle.y, weapon.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(weapon.position.x, weapon.position.y, weapon.position.z);
		DirectX::XMStoreFloat4x4(&weapon.transform, S * R * T);
	}
}

// 描画処理
void HitStopScene::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	PrimitiveRenderer* primitiveRenderer = Graphics::Instance().GetPrimitiveRenderer();
	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	// モデル描画
	Graphics::Instance().SetRenderTargets(0);
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &cameraMoai;
	modelRenderer->Render(rc, character.transform, character.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, weapon.transform, weapon.model.get(), ShaderId::Lambert);

	DirectX::XMFLOAT3 p;
	p.x = weapon.transform._41 + weapon.transform._31 * weaponHitOffset;
	p.y = weapon.transform._42 + weapon.transform._32 * weaponHitOffset;
	p.z = weapon.transform._43 + weapon.transform._33 * weaponHitOffset;
	shapeRenderer->DrawSphere(p, weaponHitRadius, { 1,1,1,1 });

	p.x = character.position.x;
	p.y = character.position.y + characterHitOffset;
	p.z = character.position.z;
	shapeRenderer->DrawSphere(p, characterHitRadius, { 1,1,1,1 });
	shapeRenderer->Render(dc, cameraMoai.GetView(), cameraMoai.GetProjection());

	// 軸描画
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::NoTestNoWrite), 0);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	primitiveRenderer->DrawAxis(weapon.transform, { 1,1,1,1 });
	primitiveRenderer->Render(dc, cameraMoai.GetView(), cameraMoai.GetProjection(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// グリッド描画
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	primitiveRenderer->DrawGrid(20, 1);
	primitiveRenderer->Render(dc, cameraMoai.GetView(), cameraMoai.GetProjection(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);



	Graphics::Instance().SetRenderTargets(1);
	rc.camera = &cameraHaniwa;
	modelRenderer->Render(rc, character.transform, character.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, weapon.transform, weapon.model.get(), ShaderId::Lambert);

	shapeRenderer->Render(dc, cameraMoai.GetView(), cameraMoai.GetProjection());

	// 軸描画
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::NoTestNoWrite), 0);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	primitiveRenderer->DrawAxis(weapon.transform, { 1,1,1,1 });
	primitiveRenderer->Render(dc, cameraMoai.GetView(), cameraMoai.GetProjection(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// グリッド描画
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	primitiveRenderer->DrawGrid(20, 1);
	primitiveRenderer->Render(dc, cameraMoai.GetView(), cameraMoai.GetProjection(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

// GUI描画処理
void HitStopScene::DrawGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();

	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 180), ImGuiCond_Once);

	if (ImGui::Begin(u8"ヒットストップ", nullptr, ImGuiWindowFlags_NoNavInputs))
	{
		ImGui::DragFloat3("CharacterPosition", &character.position.x, 0.01f);
		ImGui::DragFloat3("WeaponPosition", &weapon.position.x, 0.01f);
		ImGui::DragFloat3("WeaponAngle", &weapon.angle.x, 0.01f);

		ImGui::DragFloat("weaonHitOffset", &weaponHitOffset, 0.01f);
		ImGui::DragFloat("weaonHitRadius", &weaponHitRadius, 0.01f);
	}
	ImGui::End();
}
