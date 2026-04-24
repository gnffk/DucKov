#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class Material
{
private:
	Material(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~Material();

public:
	HRESULT Initialize(uint32_t materialtype, vector<TEXTUREINFO>& textures, const _char* ModelFilePath);
	HRESULT Bind_ShaderResource(shared_ptr<class Shader> pShader, const _char* pConstantName, uint32_t eMaterialType, uint32_t iTextureIndex);
private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };


private:
	vector<ComPtr<ID3D11ShaderResourceView>>			m_Materials[TextureType::TextureType_END];

public:
	static shared_ptr<Material> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, 
		uint32_t materialtype, vector<TEXTUREINFO>& textures, const _char* ModelFilePath);
};

NS_END