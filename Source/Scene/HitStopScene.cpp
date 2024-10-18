#include <imgui.h>
#include <ImGuizmo.h>
#include <DirectXCollision.h>
#include "Graphics.h"
#include "Collision.h"
#include "Scene/HitStopScene.h"
#include"LightManager.h"
// �R���X�g���N�^
HitStopScene::HitStopScene()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// �J�����ݒ�
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// ��p
		screenWidth / screenHeight,			// ��ʃA�X�y�N�g��
		0.1f,								// �j�A�N���b�v
		1000.0f								// �t�@�[�N���b�v
	);
	camera.SetLookAt(
		{ 4, 2, 2 },		// ���_
		{ 0, 1.5f, 1 },		// �����_
		{ 0, 1, 0 }			// ��x�N�g��
	);
	cameraController.SyncCameraToController(camera);

	character.position = { 0, 0, 0 };
	character.scale = { 0.01f, 0.01f, 0.01f };
	character.model = std::make_unique<Model>("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	weapon.position = { 0, 1, 1.5f };
	weapon.angle = { 0, 0, 0 };
	weapon.scale = { 1.0f, 1.0f, 1.0f };
	weapon.model = std::make_unique<Model>("Data/Model/RPG-Character/2Hand-Sword.mdl");

	// �J�����R���g���[���[������
	//cameraController = new CameraController();
	// ���s������ǉ�
	LightManager::Instance().Register(new Light(LightType::Directional));
	// �_������ǉ�
	{
		Light* light = new Light(LightType::Point);
		light->SetPosition(DirectX::XMFLOAT3(0, 4, 0));
		light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		light->SetRange(5.0f);
		LightManager::Instance().Register(light);
	}
	// �_������ǉ�
	{
		Light* light = new Light(LightType::Point);
		light->SetPosition(DirectX::XMFLOAT3(5, 4, 0));
		light->SetColor(DirectX::XMFLOAT4(1, 1, 0, 1));
		light->SetRange(5.0f);
		LightManager::Instance().Register(light);
	}
	// �X�|�b�g���C�g��ǉ�
	{
		Light* light = new Light(LightType::Spot);
		light->SetPosition(DirectX::XMFLOAT3(-10, 10, 10));
		light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		light->SetDirection(DirectX::XMFLOAT3(+1, -1, 0));

		light->SetRange(40.0f);
		LightManager::Instance().Register(light);
	}

}

// �X�V����
void HitStopScene::Update(float elapsedTime)
{
	// �J�����X�V����
	cameraController.Update();
	cameraController.SyncControllerToCamera(camera);


	float timeScale = 1.0f;

	// ���̓���
	weaponMotionCurrentSeconds += elapsedTime * timeScale;
	if (weaponMotionCurrentSeconds > weaponMotionSecondsLength)
	{
		weaponMotionCurrentSeconds = 0.0f;
	}
	weapon.angle.y = Easing(weaponMotionCurrentSeconds / weaponMotionSecondsLength) * DirectX::XM_2PI;

	// �L�����N�^�[�s��X�V����
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(character.scale.x, character.scale.y, character.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(character.angle.x, character.angle.y, character.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(character.position.x, character.position.y, character.position.z);
		DirectX::XMStoreFloat4x4(&character.transform, S * R * T);
	}

	// ����s��X�V����
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(weapon.scale.x, weapon.scale.y, weapon.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(weapon.angle.x, weapon.angle.y, weapon.angle.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(weapon.position.x, weapon.position.y, weapon.position.z);
		DirectX::XMStoreFloat4x4(&weapon.transform, S * R * T);
	}
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
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &camera;
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
	shapeRenderer->Render(dc, camera.GetView(), camera.GetProjection());

	// ���`��
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::NoTestNoWrite), 0);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	primitiveRenderer->DrawAxis(weapon.transform, { 1,1,1,1 });
	primitiveRenderer->Render(dc, camera.GetView(), camera.GetProjection(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// �O���b�h�`��
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	primitiveRenderer->DrawGrid(20, 1);
	primitiveRenderer->Render(dc, camera.GetView(), camera.GetProjection(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

// GUI�`�揈��
void HitStopScene::DrawGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();

	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 180), ImGuiCond_Once);

	if (ImGui::Begin(u8"�q�b�g�X�g�b�v", nullptr, ImGuiWindowFlags_NoNavInputs))
	{
		ImGui::DragFloat3("CharacterPosition", &character.position.x, 0.01f);
		ImGui::DragFloat3("WeaponPosition", &weapon.position.x, 0.01f);
		ImGui::DragFloat3("WeaponAngle", &weapon.angle.x, 0.01f);

		ImGui::DragFloat("weaonHitOffset", &weaponHitOffset, 0.01f);
		ImGui::DragFloat("weaonHitRadius", &weaponHitRadius, 0.01f);
	}
	ImGui::End();
}
