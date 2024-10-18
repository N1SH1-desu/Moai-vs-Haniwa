#pragma once

#include "RenderContext.h"
#include "Model.h"

class Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	// 開始処理
	virtual void Begin(const RenderContext& rc) = 0;

	// 更新処理
	virtual void Update(const RenderContext& rc, const ModelResource::Material& material) = 0;
	// 描画
	virtual void Draw(ID3D11DeviceContext* dc, const Model* model) = 0;


	// 終了処理
	virtual void End(const RenderContext& rc) = 0;
};
class ModelShader
{
public:
	ModelShader() {}
	virtual ~ModelShader() {}

	//// 描画開始
	//virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) = 0;

	//// 描画
	//virtual void Draw(ID3D11DeviceContext* dc, const Model* model) = 0;

	//// 描画終了
	//virtual void End(ID3D11DeviceContext* context) = 0;

	// 描画開始
	virtual void Begin(const RenderContext& rc) = 0;

	// 描画
	virtual void Draw(const RenderContext& rc, const Model* model) = 0;

	// 描画終了
	virtual void End(const RenderContext& rc) = 0;


};