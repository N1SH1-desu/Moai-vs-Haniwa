#include <fstream>
#include "Sprite.h"
#include "Misc.h"
#include "GpuResourceUtils.h"
#include <functional>

// �R���X�g���N�^
Sprite::Sprite(ID3D11Device* device)
	: Sprite(device, nullptr)
{
}

// �R���X�g���N�^
Sprite::Sprite(ID3D11Device* device, const char* filename)
{
	HRESULT hr = S_OK;

	// ���_�o�b�t�@�̐���
	{
		// ���_�o�b�t�@���쐬���邽�߂̐ݒ�I�v�V����
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(Vertex) * 4;
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		// ���_�o�b�t�@�I�u�W�F�N�g�̐���
		hr = device->CreateBuffer(&buffer_desc, nullptr, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ���_�V�F�[�_�[
	{
		// ���̓��C�A�E�g
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = GpuResourceUtils::LoadVertexShader(
			device,
			"Data/Shader/SpriteVS.cso",
			inputElementDesc,
			ARRAYSIZE(inputElementDesc),
			inputLayout.GetAddressOf(),
			vertexShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	}

	// �s�N�Z���V�F�[�_�[
	{
		hr = GpuResourceUtils::LoadPixelShader(
			device,
			"Data/Shader/SpritePS.cso",
			pixelShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// �u�����h�X�e�[�g
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		
		HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// �[�x�X�e���V���X�e�[�g
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ���X�^���C�U�[�X�e�[�g
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// �T���v���X�e�[�g
	{
		D3D11_SAMPLER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// �e�N�X�`���̐���	
	if (filename != nullptr)
	{
		// �e�N�X�`���t�@�C���ǂݍ���
		D3D11_TEXTURE2D_DESC desc;
		hr = GpuResourceUtils::LoadTexture(device, filename, shaderResourceView.GetAddressOf(), &desc);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		textureWidth = static_cast<float>(desc.Width);
		textureHeight = static_cast<float>(desc.Height);
	}
	else
	{
		// �_�~�[�e�N�X�`������
		D3D11_TEXTURE2D_DESC desc;
		hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFFFFFF, shaderResourceView.GetAddressOf(),
			&desc);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		textureWidth = static_cast<float>(desc.Width);
		textureHeight = static_cast<float>(desc.Height);
	}
}

// �`����s
void Sprite::Render(ID3D11DeviceContext* dc,
	float dx, float dy,					// ����ʒu
	float dz,							// ���s
	float dw, float dh,					// ���A����
	float sx, float sy,					// �摜�؂蔲���ʒu
	float sw, float sh,					// �摜�؂蔲���T�C�Y
	float angle,						// �p�x
	float r, float g, float b, float a	// �F
	) const
{
	// ���_���W
	DirectX::XMFLOAT2 positions[] = {
		DirectX::XMFLOAT2(dx,      dy),			// ����
		DirectX::XMFLOAT2(dx + dw, dy),			// �E��
		DirectX::XMFLOAT2(dx,      dy + dh),	// ����
		DirectX::XMFLOAT2(dx + dw, dy + dh),	// �E��
	};

	// �e�N�X�`�����W
	DirectX::XMFLOAT2 texcoords[] = {
		DirectX::XMFLOAT2(sx,      sy),			// ����
		DirectX::XMFLOAT2(sx + sw, sy),			// �E��
		DirectX::XMFLOAT2(sx,      sy + sh),	// ����
		DirectX::XMFLOAT2(sx + sw, sy + sh),	// �E��
	};

	// �X�v���C�g�̒��S�ŉ�]�����邽�߂ɂS���_�̒��S�ʒu��
	// ���_(0, 0)�ɂȂ�悤�Ɉ�U���_���ړ�������B
	float mx = dx + dw * 0.5f;
	float my = dy + dh * 0.5f;
	for (auto& p : positions)
	{
		p.x -= mx;
		p.y -= my;
	}

	// ���_����]������
	float theta = DirectX::XMConvertToRadians(angle);
	float c = cosf(theta);
	float s = sinf(theta);
	for (auto& p : positions)
	{
		DirectX::XMFLOAT2 r = p;
		p.x = c * r.x + -s * r.y;
		p.y = s * r.x + c * r.y;
	}

	// ��]�̂��߂Ɉړ����������_�����̈ʒu�ɖ߂�
	for (auto& p : positions)
	{
		p.x += mx;
		p.y += my;
	}

	// ���ݐݒ肳��Ă���r���[�|�[�g����X�N���[���T�C�Y���擾����B
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);
	float screenWidth = viewport.Width;
	float screenHeight = viewport.Height;

	// �X�N���[�����W�n����NDC���W�n�֕ϊ�����B
	for (DirectX::XMFLOAT2& p : positions)
	{
		p.x = 2.0f * p.x / screenWidth - 1.0f;
		p.y = 1.0f - 2.0f * p.y / screenHeight;
	}

	// ���_�o�b�t�@�̓��e�̕ҏW���J�n����B
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// ���_�o�b�t�@�̓��e��ҏW
	Vertex* v = static_cast<Vertex*>(mappedSubresource.pData);
	for (int i = 0; i < 4; ++i)
	{
		v[i].position.x = positions[i].x;
		v[i].position.y = positions[i].y;
		v[i].position.z = dz;

		v[i].color.x = r;
		v[i].color.y = g;
		v[i].color.z = b;
		v[i].color.w = a;

		v[i].texcoord.x = texcoords[i].x / textureWidth;
		v[i].texcoord.y = texcoords[i].y / textureHeight;
	}

	// ���_�o�b�t�@�̓��e�̕ҏW���I������B
	dc->Unmap(vertexBuffer.Get(), 0);

	// GPU�ɕ`�悷�邽�߂̃f�[�^��n��
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetInputLayout(inputLayout.Get());
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	// �`��
	dc->Draw(4, 0);
}

// �`����s�i�e�N�X�`���؂蔲���w��Ȃ��j
void Sprite::Render(ID3D11DeviceContext* dc,
	float dx, float dy,					// ����ʒu
	float dz,							// ���s
	float dw, float dh,					// ���A����
	float angle,						// �p�x
	float r, float g, float b, float a	// �F
	) const
{
	Render(dc, dx, dy, dz, dw, dh, 0, 0, textureWidth, textureHeight, angle, r, g, b, a);
}

void Sprite::Render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle, float r, float g, float b, float a) const
{
	{
		// ���ݐݒ肳��Ă���r���[�|�[�g����X�N���[���T�C�Y���擾����B
		D3D11_VIEWPORT viewport;
		UINT numViewports = 1;
		immediate_context->RSGetViewports(&numViewports, &viewport);
		float screen_width = viewport.Width;
		float screen_height = viewport.Height;
		
		
		// �X�v���C�g���\������S���_�̃X�N���[�����W���v�Z����
		DirectX::XMFLOAT2 positions[] = {
			DirectX::XMFLOAT2(dx,      dy),			// ����
			DirectX::XMFLOAT2(dx + dw, dy),			// �E��
			DirectX::XMFLOAT2(dx,      dy + dh),	// ����
			DirectX::XMFLOAT2(dx + dw, dy + dh),	// �E��
		};

		// �X�v���C�g���\������S���_�̃e�N�X�`�����W���v�Z����
		DirectX::XMFLOAT2 texcoords[] = {
			DirectX::XMFLOAT2(sx,      sy),			// ����
			DirectX::XMFLOAT2(sx + sw, sy),			// �E��
			DirectX::XMFLOAT2(sx,      sy + sh),	// ����
			DirectX::XMFLOAT2(sx + sw, sy + sh),	// �E��
		};

		// �X�v���C�g�̒��S�ŉ�]�����邽�߂ɂS���_�̒��S�ʒu��
		// ���_(0, 0)�ɂȂ�悤�Ɉ�U���_���ړ�������B
		float mx = dx + dw * 0.5f;
		float my = dy + dh * 0.5f;
		for (auto& p : positions)
		{
			p.x -= mx;
			p.y -= my;
		}

		// ���_����]������
		const float PI = 3.141592653589793f;
		float theta = angle * (PI / 180.0f);	// �p�x�����W�A��(��)�ɕϊ�
		float c = cosf(theta);
		float s = sinf(theta);
		for (auto& p : positions)
		{
			DirectX::XMFLOAT2 r = p;
			p.x = c * r.x + -s * r.y;
			p.y = s * r.x + c * r.y;
		}

		// ��]�̂��߂Ɉړ����������_�����̈ʒu�ɖ߂�
		for (auto& p : positions)
		{
			p.x += mx;
			p.y += my;
		}

		// �X�N���[�����W�n����NDC���W�n�֕ϊ�����B
		for (auto& p : positions)
		{
			p.x = 2.0f * p.x / screen_width - 1.0f;
			p.y = 1.0f - 2.0f * p.y / screen_height;
		}

		// ���_�o�b�t�@�̓��e�̕ҏW���J�n����B
		D3D11_MAPPED_SUBRESOURCE mappedBuffer;
		HRESULT hr = immediate_context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// pData��ҏW���邱�ƂŒ��_�f�[�^�̓��e�����������邱�Ƃ��ł���B
		Vertex* v = static_cast<Vertex*>(mappedBuffer.pData);
		for (int i = 0; i < 4; ++i)
		{
			v[i].position.x = positions[i].x;
			v[i].position.y = positions[i].y;
			v[i].position.z = 0.0f;

			v[i].color.x = r;
			v[i].color.y = g;
			v[i].color.z = b;
			v[i].color.w = a;

			v[i].texcoord.x = texcoords[i].x / textureWidth;
			v[i].texcoord.y = texcoords[i].y / textureHeight;
		}

		// ���_�o�b�t�@�̓��e�̕ҏW���I������B
		immediate_context->Unmap(vertexBuffer.Get(), 0);
	}

	{
		// �p�C�v���C���ݒ�
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		immediate_context->IASetInputLayout(inputLayout.Get());

		immediate_context->RSSetState(rasterizerState.Get());

		immediate_context->VSSetShader(vertexShader.Get(), nullptr, 0);
		immediate_context->PSSetShader(pixelShader.Get(), nullptr, 0);

		immediate_context->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
		immediate_context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

		// �`��
		immediate_context->Draw(4, 0);
	}
}

void Sprite::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)
{
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	// Set each sprite's vertices coordinate to screen space
	//
	//  (x0, y0) *----* (x1, y1) 
	//	         |   /|
	//	         |  / |
	//	         | /  |
	//	         |/   |
	//  (x2, y2) *----* (x3, y3) 

	// left-top
	float x0{ dx };
	float y0{ dy };
	// right-top
	float x1{ dx + dw };
	float y1{ dy };
	// left-bottom
	float x2{ dx };
	float y2{ dy + dh };
	// right-bottom
	float x3{ dx + dw };
	float y3{ dy + dh };

#if 1
	//auto rotate = [](float& x, float& y, float cx, float cy, float angle)
	std::function<void(float&, float&, float, float, float)> rotate = [](float& x, float& y, float cx, float cy, float angle)
		{
			x -= cx;
			y -= cy;

			float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
			float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
			float tx{ x }, ty{ y };
			x = cos * tx + -sin * ty;
			y = sin * tx + cos * ty;

			x += cx;
			y += cy;
		};
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	rotate(x0, y0, cx, cy, angle);
	rotate(x1, y1, cx, cy, angle);
	rotate(x2, y2, cx, cy, angle);
	rotate(x3, y3, cx, cy, angle);
#else
	// Translate sprite's centre to origin (rotate centre)
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	x0 -= cx;
	y0 -= cy;
	x1 -= cx;
	y1 -= cy;
	x2 -= cx;
	y2 -= cy;
	x3 -= cx;
	y3 -= cy;

	// Rotate each sprite's vertices by angle
	float tx, ty;
	float cos{ cosf(XMConvertToRadians(angle)) };
	float sin{ sinf(XMConvertToRadians(angle)) };
	tx = x0;
	ty = y0;
	x0 = cos * tx + -sin * ty;
	y0 = sin * tx + cos * ty;
	tx = x1;
	ty = y1;
	x1 = cos * tx + -sin * ty;
	y1 = sin * tx + cos * ty;
	tx = x2;
	ty = y2;
	x2 = cos * tx + -sin * ty;
	y2 = sin * tx + cos * ty;
	tx = x3;
	ty = y3;
	x3 = cos * tx + -sin * ty;
	y3 = sin * tx + cos * ty;

	// Translate sprite's centre to original position
	x0 += cx;
	y0 += cy;
	x1 += cx;
	y1 += cy;
	x2 += cx;
	y2 += cy;
	x3 += cx;
	y3 += cy;
#endif

	// Convert to NDC space
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = immediate_context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mapped_subresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0 , 0 };
		vertices[1].position = { x1, y1 , 0 };
		vertices[2].position = { x2, y2 , 0 };
		vertices[3].position = { x3, y3 , 0 };

		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };

		// UNIT.06
		vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
		vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
		vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
		vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
	}

	immediate_context->Unmap(vertexBuffer.Get(), 0);

	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	immediate_context->IASetInputLayout(inputLayout.Get());

	immediate_context->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixelShader.Get(), nullptr, 0);

	immediate_context->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	immediate_context->Draw(4, 0);
}

void Sprite::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh)
{
	render(immediate_context, dx, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::textout(ID3D11DeviceContext* dc, std::string s, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle, float r, float g, float b, float a)
{
	float tw = static_cast<float>(GetTextureWidth() / 16);
	float th = static_cast<float>(GetTextureHeight() / 16);
	dc->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF);

	float carriage = 0.0f;
	for (const char c : s)
	{
		Render(dc, dx + carriage, dy, dw, dh,
			tw * (c & 0x0F), th * (c >> 4), sw, sh, angle, r, g, b, a);
		carriage += dw;
	}
}



