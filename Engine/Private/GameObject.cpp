#include "GameObject.h"

#include "Transform.h"
#include "GameInstance.h"


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

    m_ObjectName = pDesc->m_strName;



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

HRESULT GameObject::Add_Component(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, shared_ptr<Component>& pOut, void* pArg)
{
    if (nullptr != Find_Component(strComponentTag))
        return E_FAIL;

    auto	pComponent = dynamic_pointer_cast<Component>(CGameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));
    if (nullptr == pComponent)
        return E_FAIL;

    m_Components.emplace(strComponentTag, pComponent);

    pOut = pComponent;
    return S_OK;
}

shared_ptr<class Component> GameObject::Find_Component(const _wstring& strComponentTag)
{
    auto	iter = m_Components.find(strComponentTag);
    if (iter == m_Components.end())
        return nullptr;

    return iter->second;
}

