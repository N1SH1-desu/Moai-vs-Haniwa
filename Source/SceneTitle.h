#pragma once
#include "Sprite.h"
#include "Scene.h"
// �^�C�g���V�[��
class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}
	// ������
	void Initialize();
	// �I����
	void Finalize();
	// �X�V����
	void Update(float elapsedTime) override;
	// �`�揈��
	void Render();
private:
	Sprite* sprite = nullptr;
	std::unique_ptr<Scene>	scene;
	Camera					camera;
};