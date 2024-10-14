#pragma once
#include "Sprite.h"
#include "Scene.h"
// タイトルシーン
class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}
	// 初期化
	void Initialize();
	// 終了化
	void Finalize();
	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render();
private:
	Sprite* sprite = nullptr;
	std::unique_ptr<Scene>	scene;
	Camera					camera;
};