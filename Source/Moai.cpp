#include "Moai.h"

namespace Characters
{
	Moai::Moai(const std::string modelPath, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale)
		:
		Artifact(modelPath, position, scale)
	{
		gamePad.SetSlot(0);

		cameraController.camera.SetPerspectiveFov(
			DirectX::XMConvertToRadians(45),	// 画角
			(1280.0f / 2.0f) / 720.0f,					// 画面アスペクト比
			0.1f,								// ニアクリップ
			1000.0f								// ファークリップ
		);

		cameraController.camera.SetLookAt(
			{ 4, 2, 2 },
			{ 0, 1.5f, 1 },
			{ 0, 1, 0 }
		);

		cameraController.SyncCameraToCon();
	}
}
