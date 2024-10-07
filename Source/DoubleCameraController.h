#pragma once
#include "Camera.h"
#include "FreeCameraController.h"

class DoubleCameraController
{
public:
	enum CAMERA_IDENTIFIER
	{
		MOAI = 0,
		HANIWA,
		COUNT,
	};

public:
	DoubleCameraController();
	~DoubleCameraController() = default;

	void SetLookAt(CAMERA_IDENTIFIER& identifier, const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);
	void Update();

	const Camera& GetCamera(CAMERA_IDENTIFIER identifier);

private:
	struct CameraPair
	{
		Camera camera;
		FreeCameraController cameraController;

		void SyncCameraToCon() { cameraController.SyncCameraToController(camera); }
		void SyncConToCamera() { cameraController.SyncControllerToCamera(camera); }

		void Update()
		{
			cameraController.Update();
			cameraController.SyncControllerToCamera(camera);
		}
	};
	CameraPair cameras[CAMERA_IDENTIFIER::COUNT];
};
