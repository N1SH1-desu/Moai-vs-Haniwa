#include "Haniwa.h"

namespace Characters
{
	Haniwa::Haniwa(const std::string modelPath, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale)
		:
		Artifact(modelPath, position, scale)
	{
		gamePad.SetSlot(1);

		cameraController.camera.SetPerspectiveFov(
			DirectX::XMConvertToRadians(45),	// ��p
			(1280.0f / 2.0f) / 720.0f,			// ��ʃA�X�y�N�g��
			0.1f,								// �j�A�N���b�v
			1000.0f								// �t�@�[�N���b�v
		);

		cameraController.camera.SetLookAt(
			{ -4, 2, -2 },
			position,
			{ 0, 1, 0 }
		);

		cameraController.SyncCameraToCon();
	}
}
