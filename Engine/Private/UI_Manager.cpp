#include "UI_Manager.h"

NS_BEGIN(Engine)

UI_Manager::~UI_Manager()
{
    Clear();
}

HRESULT UI_Manager::Initialize()
{
    return S_OK;
}

void UI_Manager::Priority_Update(_float fTimeDelta)
{
    for (auto& Pair : m_UIGroups)
    {
        for (auto& pUIObject : Pair.second)
        {
            if (nullptr == pUIObject)
                continue;

            pUIObject->Priority_Update(fTimeDelta);
        }
    }
}

void UI_Manager::Update(_float fTimeDelta)
{
    for (auto& Pair : m_UIGroups)
    {
        for (auto& pUIObject : Pair.second)
        {
            if (nullptr == pUIObject)
                continue;

            pUIObject->Update(fTimeDelta);
        }
    }
}

void UI_Manager::Late_Update(_float fTimeDelta)
{
    for (auto& Pair : m_UIGroups)
    {
        for (auto& pUIObject : Pair.second)
        {
            if (nullptr == pUIObject)
                continue;
            CGameInstance::Get().Add_RenderObject(RENDERGROUP::UI, pUIObject);

            pUIObject->Late_Update(fTimeDelta);
        }
    }
}

HRESULT UI_Manager::Render()
{
  

    return S_OK;
}

HRESULT UI_Manager::Add_UIObject( const wstring& strUIGroup, shared_ptr<UIObject> pUIObject)
{
    if (nullptr == pUIObject)
        return E_FAIL;

    m_UIGroups[strUIGroup].push_back(pUIObject);

    if (m_strMainUIGroup.empty())
        m_strMainUIGroup = strUIGroup;

    return S_OK;
}

vector<shared_ptr<UIObject>>* UI_Manager::Find_UIGroup( const wstring& strUIGroup)
{
    auto iter = m_UIGroups.find(strUIGroup);

    if (iter == m_UIGroups.end())
        return nullptr;

    return &iter->second;
}

shared_ptr<UIObject> UI_Manager::Find_UIObject( const wstring& strUIGroup, const wstring& strUIObjectTag)
{
    auto pGroup = Find_UIGroup(strUIGroup);

    if (nullptr == pGroup)
        return nullptr;

    for (auto& pUIObject : *pGroup)
    {
        if (nullptr == pUIObject)
            continue;

        if (pUIObject->GetObjectINFO().m_strName == strUIObjectTag)
            return pUIObject;
    }

    return nullptr;
}

void UI_Manager::Set_MainUIGroup(const wstring& strUIGroup)
{
    m_strMainUIGroup = strUIGroup;
}

const wstring& UI_Manager::Get_MainUIGroup() const
{
    return m_strMainUIGroup;
}

void UI_Manager::Clear()
{
    m_UIGroups.clear();
    m_strMainUIGroup.clear();
}

void UI_Manager::Clear_Group(const wstring& strUIGroup)
{
    auto iter = m_UIGroups.find(strUIGroup);

    if (iter == m_UIGroups.end())
        return;

    iter->second.clear();
    m_UIGroups.erase(iter);

    if (m_strMainUIGroup == strUIGroup)
        m_strMainUIGroup.clear();
}

unique_ptr<UI_Manager> UI_Manager::Create()
{
    unique_ptr<UI_Manager> pInstance(new UI_Manager());
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : UI_Manager");
        return nullptr;
    }

    return pInstance;
}

NS_END