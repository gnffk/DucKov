#pragma once

#include "Prototype.h"

NS_BEGIN(Engine)

class ENGINE_DLL Component abstract : public Prototype
{
protected:
	Component(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Component(const Component& Prototype);
public:
	virtual ~Component() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

protected:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };


protected:

public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;

};

NS_END