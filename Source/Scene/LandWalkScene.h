#pragma once

#include <memory>
#include "Scene.h"
#include "Camera.h"
#include "FreeCameraController.h"
#include "Model.h"
#include <set>

class LandWalkScene : public Scene
{
public:
	LandWalkScene();
	~LandWalkScene() override = default;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(float elapsedTime) override;

	// GUI�`�揈��
	void DrawGUI() override;

	// �G�l�~�[���擾
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// �G�l�~�[�擾
	LandWalkScene* GetEnemy(int index) { return enemies.at(index); }

private:
	struct Object
	{
		bool					onGround = false;
		DirectX::XMFLOAT3		velocity = { 0, 0, 0 };
		DirectX::XMFLOAT3		position = { 0, 0, 0 };
		DirectX::XMFLOAT3		angle = { 0, 0, 0 };
		DirectX::XMFLOAT3		scale = { 1, 1, 1 };
		DirectX::XMFLOAT4X4		transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		std::unique_ptr<Model>	model;

		void UpdateTransform()
		{
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
			DirectX::XMMATRIX WorldTransform = S * R * T;
			DirectX::XMStoreFloat4x4(&transform, WorldTransform);
		}
	};

	// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
	//void CollisionPlayerVsEnemies();

	Camera								camera;
	FreeCameraController				cameraController;
	Object								player;
	Object								stage;
	Object                              enemy;
	float								gravity = 10.0f;
	float								acceleration = 30.0f;
	float								deceleration = 20.0f;
	float								moveSpeed = 5.0f;
	float								turnSpeed = DirectX::XMConvertToRadians(720);
	float								jumpSpeed = 5.0f;
	float								downhillOffset = 43.0f;
	float								characterHitRadius = 0.5f;
	float								characterHitOffset = 0.5f;
	float								enemyHitOffset = 1.0f;
	float								enemyHitRadius = 0.5f;

	std::vector<LandWalkScene*>          enemies;
	//std::set<LandWalkScene*>             removes;


};
