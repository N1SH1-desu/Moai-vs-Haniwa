#include <imgui.h>
#include <ImGuizmo.h>
#include <DirectXCollision.h>
#include "Graphics.h"
#include "Collision.h"
#include "Scene/HitStopScene.h"
#include "Moai.h"
#include "Haniwa.h"

// �R���X�g���N�^
HitStopScene::HitStopScene()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	ring.position = { 0.0f, -5.0f, 0.0f };
	ring.scale = { 0.1f, 0.1f, 0.1f };
	ring.model = std::make_unique<Model>("Data/Model/Ring/rinngu.mdl");

	moai = std::make_unique<Characters::Moai>("Data/Model/moai/Moai.mdl", DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f));
	haniwa = std::make_unique<Characters::Haniwa>("Data/Model/Haniwa/haniwa.mdl", DirectX::XMFLOAT3(0.0f, 0.0f, 2.0f), DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f));
	moai.get()->SetEnemy(haniwa.get());
}

// �X�V����
void HitStopScene::Update(float elapsedTime)
{
	float timeScale = 1.0f;

	moai.get()->Update(elapsedTime);
	//haniwa.get()->Update(elapsedTime);
}

// �`�揈��
void HitStopScene::Render(float elapsedTime)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	PrimitiveRenderer* primitiveRenderer = Graphics::Instance().GetPrimitiveRenderer();
	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	// ���f���`��
	Graphics::Instance().SetRenderTargets(1);
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &moai.get()->GetCamera();
	//modelRenderer->Render(rc, character.transform, character.model.get(), ShaderId::Lambert);
	modelRenderer->Render(rc, ring.transform, ring.model.get(), ShaderId::Lambert);
	moai.get()->Render(dc, renderState, modelRenderer, &moai.get()->GetCamera());
	haniwa.get()->Render(dc, renderState, modelRenderer, &moai.get()->GetCamera());

	moai.get()->DrawDebugPrimitive(shapeRenderer);
	haniwa.get()->DrawDebugPrimitive(shapeRenderer);
	shapeRenderer->Render(dc, moai.get()->GetCamera().GetView(), moai.get()->GetCamera().GetProjection());

	Graphics::Instance().SetRenderTargets(2);
	rc.camera = &haniwa.get()->GetCamera();
	modelRenderer->Render(rc, ring.transform, ring.model.get(), ShaderId::Lambert);
	moai.get()->Render(dc, renderState, modelRenderer, &haniwa.get()->GetCamera());
	//haniwa.get()->Render(dc, renderState, modelRenderer, &haniwa.get()->GetCamera());
	//modelRenderer->Render(rc, character.transform, character.model.get(), ShaderId::Lambert);

	//shapeRenderer->Render(dc, cameraController.GetCamera(CAMERA_NAME::HANIWA).GetView(), cameraController.GetCamera(CAMERA_NAME::HANIWA).GetProjection());
}

// GUI�`�揈��
void HitStopScene::DrawGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();

	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 180), ImGuiCond_Once);

	if (ImGui::Begin(u8"�q�b�g�X�g�b�v", nullptr, ImGuiWindowFlags_NoNavInputs))
	{
		//ImGui::DragFloat3("CharacterPosition", &character.position.x, 0.01f);
		//ImGui::DragFloat3("WeaponPosition", &weapon.position.x, 0.01f);
		//ImGui::DragFloat3("WeaponAngle", &weapon.angle.x, 0.01f);

		ImGui::DragFloat("weaonHitOffset", &weaponHitOffset, 0.01f);
		ImGui::DragFloat("weaonHitRadius", &weaponHitRadius, 0.01f);
	}
	ImGui::End();
}
