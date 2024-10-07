#include "DoubleCameraController.h"

DoubleCameraController::DoubleCameraController()
{
    cameras[CAMERA_IDENTIFIER::MOAI].camera.SetPerspectiveFov(
        DirectX::XMConvertToRadians(45),	// 画角
        (1280.0f / 2.0f) / 720.0f,					// 画面アスペクト比
        0.1f,								// ニアクリップ
        1000.0f								// ファークリップ
    );
    cameras[CAMERA_IDENTIFIER::HANIWA].camera.SetPerspectiveFov(
        DirectX::XMConvertToRadians(45),	// 画角
        (1280.0f / 2.0f) / 720.0f,					// 画面アスペクト比
        0.1f,								// ニアクリップ
        1000.0f								// ファークリップ
    );

    cameras[CAMERA_IDENTIFIER::MOAI].camera.SetLookAt(
        { 4, 2, 2 },
        { 0, 1.5f, 1 },
        { 0, 1, 0 }
    );
    cameras[CAMERA_IDENTIFIER::HANIWA].camera.SetLookAt(
        { 4, 2, 2 },
        { 0, 1.5f, 1 },
        { 0, 1, 0 }
    );
    cameras[CAMERA_IDENTIFIER::MOAI].SyncCameraToCon();
    cameras[CAMERA_IDENTIFIER::HANIWA].SyncCameraToCon();
}

void DoubleCameraController::SetLookAt(CAMERA_IDENTIFIER& identifier, const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
    switch (identifier)
    {
    case DoubleCameraController::CAMERA_IDENTIFIER::MOAI:
        cameras[CAMERA_IDENTIFIER::MOAI].camera.SetLookAt(eye, focus, up);
        break;
    case DoubleCameraController::CAMERA_IDENTIFIER::HANIWA:
        cameras[CAMERA_IDENTIFIER::HANIWA].camera.SetLookAt(eye, focus, up);
        break;
    default:
        break;
    }
}

void DoubleCameraController::Update()
{
    cameras[CAMERA_IDENTIFIER::MOAI].Update();
    cameras[CAMERA_IDENTIFIER::HANIWA].Update();
}

const Camera& DoubleCameraController::GetCamera(CAMERA_IDENTIFIER identifier)
{
    switch (identifier)
    {
    case DoubleCameraController::MOAI:
        return cameras[MOAI].camera;

    case DoubleCameraController::HANIWA:
        return cameras[HANIWA].camera;
    }

    assert("Valied Identifier", false);
}
