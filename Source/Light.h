#pragma once

#include <DirectXMath.h>

struct DirectionalLight
{
	DirectX::XMFLOAT3	direction = { 0, -1, 0 };
	DirectX::XMFLOAT3	color = { 1, 1, 1 };
};

class LightManager
{
public:
	// ディレクショナルライト設定
	void SetDirectionalLight(DirectionalLight& light) { directionalLight = light; }

	// ディレクショナルライト取得
	const DirectionalLight& GetDirectionalLight() const { return directionalLight; }

private:
	DirectionalLight	directionalLight;
};
// 光源タイプ
enum class LightType
{
	Directional,	// 平行光源
	Point,			// 点光源
	Spot,			// スポットライト
};

// 光源クラス
class Light
{
public:
	Light(LightType lightType = LightType::Directional);

	// ライト情報をRenderContextに積む
	void PushRenderContext(RenderContext& rc) const;

	// デバッグ情報の表示
	void DrawDebugGUI();

	// デバッグ図形の表示
	void DrawDebugPrimitive();

	// ライトタイプ
	LightType GetLightType() { return lightType; }

	// ライトの座標
	DirectX::XMFLOAT3 GetPosition() { return position; }
	void SetPosition(DirectX::XMFLOAT3 position) { this->position = position; }

	// ライトの向き
	DirectX::XMFLOAT3 GetDirection() { return direction; }
	void SetDirection(DirectX::XMFLOAT3 direction) { this->direction = direction; }

	// 色
	DirectX::XMFLOAT4 GetColor() { return color; }
	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

private:
	LightType			lightType = LightType::Directional;		// ライトタイプ
	DirectX::XMFLOAT3	position = DirectX::XMFLOAT3(0, 0, 0);	// ライトの座標
	DirectX::XMFLOAT3	direction = DirectX::XMFLOAT3(0, -1, -1);	// ライトの向き
	DirectX::XMFLOAT4	color = DirectX::XMFLOAT4(1, 1, 1, 1);	// ライトの色
	float	range = 20.0f;		// 範囲
	float   influence = 1.0f;	// 光の影響力
	float	innerCorn = 0.99f;	// インナー
	float	outerCorn = 0.98f;	// アウター
};
