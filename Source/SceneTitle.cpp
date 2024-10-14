#include "Graphics.h"
#include "SceneTitle.h"

//#include"SceneGame.h"
//#include"SceneManager.h"
#include"Input.h"

//#include"SceneLoading.h"

// ������
void SceneTitle::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	
	// �J�����ݒ�
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),    // ��p
		screenWidth / screenHeight,            // ��ʃA�X�y�N�g��
		0.1f,                                // �j�A�N���b�v
		1000.0f                                // �t�@�[�N���b�v
	);
	camera.SetLookAt(
		{ 30, 20, 50 },        // ���_
		{ 20, 10, 20 },        // �����_
		{ 0, 1, 0 }            // ��x�N�g��
	);

	//camera.SetPerspectiveFov(0, 2, 0.1f, 1000);
	//camera.SetLookAt({0,0,0},{0,0,0},{0,0,0});
}
// �I����
void SceneTitle::Finalize()
{
	// �X�v���C�g�I����
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}
// �X�V����
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	// �Ȃɂ��{�^������������Q�[���V�[���֐؂�ւ�
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
// �`�揈��
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; // RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
	// 2D�X�v���C�g�`��
	//{
	//	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	//	float screenHeight = static_cast<float>(graphics.GetScreenHeight());
	//	float textureWidth = static_cast<float>(sprite->GetTextureWidth());
	//	float textureHeight = static_cast<float>(sprite->GetTextureHeight());
	//	// �^�C�g���X�v���C�g�`�� �o�O�����猩��
	//	sprite->Render(dc,
	//		0, 0, screenWidth, screenHeight,
	//		0, 0, textureWidth, textureHeight,
	//		0,0,
	//		1, 1, 1, 1);
	//}
	
	// 3D�`��
	{
		

		
	}

}