#pragma once

#include "RenderContext.h"
#include "Model.h"

class Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	// �J�n����
	virtual void Begin(const RenderContext& rc) = 0;

	// �X�V����
	virtual void Update(const RenderContext& rc, const ModelResource::Material& material) = 0;
	// �`��
	virtual void Draw(ID3D11DeviceContext* dc, const Model* model) = 0;


	// �I������
	virtual void End(const RenderContext& rc) = 0;
};
class ModelShader
{
public:
	ModelShader() {}
	virtual ~ModelShader() {}

	//// �`��J�n
	//virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) = 0;

	//// �`��
	//virtual void Draw(ID3D11DeviceContext* dc, const Model* model) = 0;

	//// �`��I��
	//virtual void End(ID3D11DeviceContext* context) = 0;

	// �`��J�n
	virtual void Begin(const RenderContext& rc) = 0;

	// �`��
	virtual void Draw(const RenderContext& rc, const Model* model) = 0;

	// �`��I��
	virtual void End(const RenderContext& rc) = 0;


};