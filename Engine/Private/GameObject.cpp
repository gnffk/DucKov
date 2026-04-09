#include "GameObject.h"

GameObject::GameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
}

GameObject::GameObject(const GameObject& Prototype)
	: m_pDevice{ Prototype.m_pDevice }
	, m_pContext{ Prototype.m_pContext }
{
}


HRESULT GameObject::Initialize_Prototype()
{
	

	return S_OK;
}

HRESULT GameObject::Initialize(void* pArg)
{
	m_pTransformCom = Transform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (nullptr == pArg)
		return S_OK;

	auto		pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
	lstrcpy(m_szName, pDesc->pGameObjectTag);

	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void GameObject::Priority_Update(_float fTimeDelta)
{
}

void GameObject::Update(_float fTimeDelta)
{
}

void GameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT GameObject::Render()
{
	return S_OK;
}
