#pragma once
#include "Camera.h"
#include "Input\GamePad.h"

class FreeCameraController
{
public:	
	// �J��������R���g���[���[�փp�����[�^�𓯊�����
	void SyncCameraToController(const Camera& camera);

	// �R���g���[���[����J�����փp�����[�^�𓯊�����
	void SyncControllerToCamera(Camera& camera);

	// �X�V����
	void Update(float elapsedTim);

	// �Q�[���p�b�h�ƘA��
	void GetGamePad(GamePad* ref) { gamePad = ref; }

	void SetFocus(DirectX::XMFLOAT3 pos) { focus = pos; }

private:
	GamePad* gamePad = nullptr;

	float rollSpeed = DirectX::XMConvertToRadians(90);
	float maxAngle = DirectX::XMConvertToRadians(45);
	float minAngle = DirectX::XMConvertToRadians(-45);

	DirectX::XMFLOAT3		eye;
	DirectX::XMFLOAT3		focus;
	DirectX::XMFLOAT3		up;
	DirectX::XMFLOAT3		right;
	float					distance;

	float					angleX;
	float					angleY;
};
