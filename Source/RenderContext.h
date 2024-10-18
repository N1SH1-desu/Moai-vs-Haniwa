#pragma once

#include "Camera.h"
#include "RenderState.h"
#include "Light.h"

#include <DirectXMath.h>
#include <D3D11.h>


//	���s�������
struct DirectionalLightData
{
	DirectX::XMFLOAT4	direction;	// ����
	DirectX::XMFLOAT4	color;		// �F
};

// �_�������
struct PointLightData
{
	DirectX::XMFLOAT4	position;	// ���W
	DirectX::XMFLOAT4	color;		// �F
	float				range;		// �͈�
	float				influence;//���̉e����
	DirectX::XMFLOAT2	dummy;
};

// �_�����̍ő吔
static	constexpr	int	PointLightMax = 8;

// �X�|�b�g���C�g���
struct SpotLightData
{
	DirectX::XMFLOAT4	position;	// ���W
	DirectX::XMFLOAT4	direction;	// ����
	DirectX::XMFLOAT4	color;		// �F
	float			range;		// �͈�
	float			innerCorn; 	// �C���i�[�p�x�͈�
	float			outerCorn; 	// �A�E�^�[�p�x�͈�
	//float			dummy;
	float			influence;
};

// �X�|�b�g���C�g�̍ő吔
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


	//	���C�g���
	DirectX::XMFLOAT4		ambientLightColor;
	DirectionalLightData	directionalLightData;
	PointLightData		pointLightData[PointLightMax];	// �_�������
	int					pointLightCount = 0;			// �_������
	SpotLightData			spotLightData[SpotLightMax];	// �X�|�b�g���C�g���
	int					spotLightCount = 0;			// �X�|�b�g���C�g��

};
