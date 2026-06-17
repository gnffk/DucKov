#include "Layer.h"

HRESULT Layer::Add_GameObject(shared_ptr<GameObject> pGameObject)
{
    if (nullptr == pGameObject)
        return E_FAIL;
    

    m_GameObjects.push_back(pGameObject);

    return S_OK;
}

void Layer::Priority_Update(_float fTimeDelta)
{
    for (auto& pGameObject : m_GameObjects)
    {
        if (nullptr != pGameObject)
            pGameObject->Priority_Update(fTimeDelta);
    }
}

void Layer::Update(_float fTimeDelta)
{
    for (auto& pGameObject : m_GameObjects)
    {
        if (nullptr != pGameObject)
            pGameObject->Update(fTimeDelta);
    }
}

void Layer::Late_Update(_float fTimeDelta)
{
    for (auto& pGameObject : m_GameObjects)
    {
        if (pGameObject == nullptr)
            continue;

        if (pGameObject->Get_Dead())
            continue;

        pGameObject->Late_Update(fTimeDelta);
    }
}

void Layer::Clear_DeadObjects()
{
    for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
    {
        if (*iter == nullptr || (*iter)->Get_Dead())
        {
            iter = m_GameObjects.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

unique_ptr<Layer> Layer::Create()
{
    return unique_ptr<Layer>(new Layer());
}
