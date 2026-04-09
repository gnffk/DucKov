#include "Prototype_Manager.h"

Prototype_Manager::Prototype_Manager()
{
}

Prototype_Manager::~Prototype_Manager()
{
}

HRESULT Prototype_Manager::Initialize(uint32_t iNumLevels)
{
	m_pPrototypes = unique_ptr<PROTOTYPES[]>(new PROTOTYPES[iNumLevels]);

	m_iNumLevels = iNumLevels;

	return S_OK;
}

void Prototype_Manager::Clear(uint32_t iClearLevelIndex)
{
	if (iClearLevelIndex >= m_iNumLevels ||
		nullptr == m_pPrototypes)
		return;

	for (auto& Pair : m_pPrototypes[iClearLevelIndex])
	{
		Pair.second.reset();
	}
	m_pPrototypes[iClearLevelIndex].clear();
}


HRESULT Prototype_Manager::Add_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, unique_ptr<Prototype>  pPrototype)
{
	if (nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
		return E_FAIL;

	size_t a = m_pPrototypes[iLevelIndex].size();
	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, std::move(pPrototype));
	size_t b = m_pPrototypes[iLevelIndex].size();

	return S_OK;
}

shared_ptr<Prototype> Prototype_Manager::Clone_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	Prototype* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	return pPrototype->Clone(pArg);
}

Prototype* Prototype_Manager::Find_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;



	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second.get();
}

unique_ptr<Prototype_Manager> Prototype_Manager::Create(uint32_t iNumLevels)
{
	auto		pInstance = unique_ptr<Prototype_Manager>(new Prototype_Manager());

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CPrototype_Manager");
		return nullptr;
	}

	return pInstance;
}

