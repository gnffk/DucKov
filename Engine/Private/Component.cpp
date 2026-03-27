#include "Component.h"

Component::Component(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
}

Component::Component(const Component& Prototype)
	: m_pDevice{ Prototype.m_pDevice }
	, m_pContext{ Prototype.m_pContext }
{
}


HRESULT Component::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Component::Initialize(void* pArg)
{
	return S_OK;
}
