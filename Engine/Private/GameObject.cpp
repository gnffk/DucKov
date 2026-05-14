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
    , Object_INFO{ Prototype.Object_INFO}
{
}

GameObject::~GameObject() {

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
 
    Object_INFO.ObjectType = pDesc->ObjectType;
    Object_INFO.LevelIndex = pDesc->LevelIndex;
    Object_INFO.m_strName = pDesc->m_strName;
    Object_INFO.m_strPrototypeObjectName = pDesc->m_strPrototypeObjectName;
    Object_INFO.m_strPrototypeBaseName = pDesc->m_strPrototypeBaseName;
    Object_INFO.pCameraType = pDesc->pCameraType;
    Object_INFO.fSpeedPerSec = pDesc->fSpeedPerSec;
    Object_INFO.fRotationPerSec = pDesc->fRotationPerSec;

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

HRESULT GameObject::Add_Component(const _wstring& strComponentTag, shared_ptr<Component> pComponent)
{
    if (nullptr != Find_Component(strComponentTag))
        return E_FAIL;

    m_Components.emplace(strComponentTag, pComponent);

    return S_OK;
}

shared_ptr<class Component> GameObject::Find_Component(const _wstring& strComponentTag)
{
    auto	iter = m_Components.find(strComponentTag);
    if (iter == m_Components.end())
        return nullptr;

    return iter->second;
}

