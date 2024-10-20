#pragma once

#include <windows.h>
#include "HighResolutionTimer.h"
#include "Scene.h"
#include "Audio.h"
class Framework
{
public:
	Framework(HWND hWnd);
	~Framework();

private:
	void Update(float elapsedTime);
	void Render(float elapsedTime);

	template<class T>
	void ChangeSceneButtonGUI(const char* name);

	void SceneSelectGUI();

	void CalculateFrameStats();

public:
	int Run();
	LRESULT CALLBACK HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int scene_table;
private:
	const HWND				hWnd;
	HighResolutionTimer		timer;
	std::unique_ptr<Scene>	scene;
	Audio audio;
	int scene_timer;
	int ruleTimer;
};

