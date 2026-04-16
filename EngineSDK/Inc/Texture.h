
#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Texture final : public Component
{
private:
	Texture(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~Texture();

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, uint32_t iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShaderResource(shared_ptr<class Shader> pShader, const _char* pConstantName, uint32_t iTextureIndex);

private:
	uint32_t									m_iNumTextures = {};
	vector<ComPtr<ID3D11ShaderResourceView>>			m_Textures;

public:
	static unique_ptr<Texture> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pTextureFilePath, uint32_t iNumTextures);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END