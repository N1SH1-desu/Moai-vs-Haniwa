#pragma once

#include "Camera.h"
#include "RenderState.h"
#include "Light.h"

#include <DirectXMath.h>
#include <D3D11.h>


//	平行光源情報
struct DirectionalLightData
{
	DirectX::XMFLOAT4	direction;	// 向き
	DirectX::XMFLOAT4	color;		// 色
};

// 点光源情報
struct PointLightData
{
	DirectX::XMFLOAT4	position;	// 座標
	DirectX::XMFLOAT4	color;		// 色
	float				range;		// 範囲
	float				influence;//光の影響力
	DirectX::XMFLOAT2	dummy;
};

// 点光源の最大数
static	constexpr	int	PointLightMax = 8;

// スポットライト情報
struct SpotLightData
{
	DirectX::XMFLOAT4	position;	// 座標
	DirectX::XMFLOAT4	direction;	// 向き
	DirectX::XMFLOAT4	color;		// 色
	float			range;		// 範囲
	float			innerCorn; 	// インナー角度範囲
	float			outerCorn; 	// アウター角度範囲
	//float			dummy;
	float			influence;
};

// スポットライトの最大数
static	constexpr	int	SpotLightMax = 8;


struct RenderContext
{
	ID3D11DeviceContext*	deviceContext;
	DirectX::XMFLOAT4		viewPosition;
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;
	DirectX::XMFLOAT4		lightDirection;
	const RenderState*		renderState;
	const Camera*			camera;
	const LightManager*		lightManager = nullptr;


	//	ライト情報
	DirectX::XMFLOAT4		ambientLightColor;
	DirectionalLightData	directionalLightData;
	PointLightData		pointLightData[PointLightMax];	// 点光源情報
	int					pointLightCount = 0;			// 点光源数
	SpotLightData			spotLightData[SpotLightMax];	// スポットライト情報
	int					spotLightCount = 0;			// スポットライト数

};
