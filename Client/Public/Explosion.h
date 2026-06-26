
#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Particle_Point;
NS_END


NS_BEGIN(Client)

class Explosion final : public PartObject
{
private:
	Explosion(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Explosion(const Explosion& Prototype);
public:
	virtual ~Explosion();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<VIBuffer_Particle_Point>	m_pVIBufferCom = { nullptr };
	shared_ptr<Texture>				m_pTextureCom = { nullptr };
	shared_ptr<Shader>					m_pShaderCom = { nullptr };

private:
	HRESULT Ready_Components();
private:
	_float m_fAnimTime = 0.f;
	_float m_fAnimFPS = 24.f;

	int32_t m_iFrameIndex = 0;
	int32_t m_iFrameCount = 64;
	int32_t m_iFrameCol = 8;
	int32_t m_iFrameRow = 8;

	_float m_fLifeTime = 64.f / 24.f;
	_bool m_bLoop = false;
public:
	static unique_ptr<Explosion> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END