#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class Shader;
class VIBuffer_Terrain;
NS_END


NS_BEGIN(Client)

class Terrain final : public GameObject
{
private:
	Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Terrain(const Terrain& Prototype);
public:
	virtual ~Terrain();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<VIBuffer_Terrain>	m_pVIBufferCom = { nullptr };
	shared_ptr<Texture>			m_pTextureCom = { nullptr };
	shared_ptr<Shader>				m_pShaderCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static unique_ptr<Terrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END