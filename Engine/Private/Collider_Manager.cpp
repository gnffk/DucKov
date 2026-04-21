#include "Collider_Manager.h"
#include "BaseCollider.h"


Collider_Manager::~Collider_Manager()
{
}
HRESULT Collider_Manager::Initialize() {
	return S_OK;
}

HRESULT Collider_Manager::Add_Collider(wstring GroupTag, BaseCollider* pCollider)
{
    if(FAILED(Find_Collider(GroupTag, pCollider)))
        return E_FAIL;

	return S_OK;
}

HRESULT Collider_Manager::Find_Collider(wstring GroupTag, BaseCollider* pCollider)
{
    if (pCollider == nullptr)
        return E_FAIL;

    auto& vec = m_Colliders[GroupTag];

    auto it = std::find_if(vec.begin(), vec.end(),
        [pCollider](BaseCollider* other)
        {
            if (other == nullptr)
                return false;

            return other->Get_Tag() == pCollider->Get_Tag();
        });

    if (it != vec.end())
        return E_FAIL; 

    vec.push_back(pCollider);
    return S_OK;
}

HRESULT Collider_Manager::Clear()
{
    m_Colliders.clear();

    return S_OK;
}

void Collider_Manager::Render() {
    for (auto& ColliderGroup : m_Colliders) {
        for (auto& Collider : ColliderGroup.second) {
            Collider->Bind_Resources();
            Collider->Render();
       }
    }
}
unique_ptr<Collider_Manager> Collider_Manager::Create() {
	auto		pInstance = unique_ptr<Collider_Manager>(new Collider_Manager());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Collider_Manager");
		return nullptr;
	}

	return pInstance;
}