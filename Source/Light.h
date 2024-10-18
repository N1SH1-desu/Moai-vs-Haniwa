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
	// �f�B���N�V���i�����C�g�ݒ�
	void SetDirectionalLight(DirectionalLight& light) { directionalLight = light; }

	// �f�B���N�V���i�����C�g�擾
	const DirectionalLight& GetDirectionalLight() const { return directionalLight; }

private:
	DirectionalLight	directionalLight;
};
// �����^�C�v
enum class LightType
{
	Directional,	// ���s����
	Point,			// �_����
	Spot,			// �X�|�b�g���C�g
};

// �����N���X
class Light
{
public:
	Light(LightType lightType = LightType::Directional);

	// ���C�g����RenderContext�ɐς�
	void PushRenderContext(RenderContext& rc) const;

	// �f�o�b�O���̕\��
	void DrawDebugGUI();

	// �f�o�b�O�}�`�̕\��
	void DrawDebugPrimitive();

	// ���C�g�^�C�v
	LightType GetLightType() { return lightType; }

	// ���C�g�̍��W
	DirectX::XMFLOAT3 GetPosition() { return position; }
	void SetPosition(DirectX::XMFLOAT3 position) { this->position = position; }

	// ���C�g�̌���
	DirectX::XMFLOAT3 GetDirection() { return direction; }
	void SetDirection(DirectX::XMFLOAT3 direction) { this->direction = direction; }

	// �F
	DirectX::XMFLOAT4 GetColor() { return color; }
	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

private:
	LightType			lightType = LightType::Directional;		// ���C�g�^�C�v
	DirectX::XMFLOAT3	position = DirectX::XMFLOAT3(0, 0, 0);	// ���C�g�̍��W
	DirectX::XMFLOAT3	direction = DirectX::XMFLOAT3(0, -1, -1);	// ���C�g�̌���
	DirectX::XMFLOAT4	color = DirectX::XMFLOAT4(1, 1, 1, 1);	// ���C�g�̐F
	float	range = 20.0f;		// �͈�
	float   influence = 1.0f;	// ���̉e����
	float	innerCorn = 0.99f;	// �C���i�[
	float	outerCorn = 0.98f;	// �A�E�^�[
};
