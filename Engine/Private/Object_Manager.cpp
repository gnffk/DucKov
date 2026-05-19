#include "Object_Manager.h"
#include "Layer.h"

#include "GameInstance.h"

Object_Manager::~Object_Manager()
{
}

HRESULT Object_Manager::Initialize(uint32_t iNumLevels)
{
	m_pLayers = unique_ptr<LAYERS[]>(new LAYERS[iNumLevels]);

	m_iNumLevels = iNumLevels;

	return S_OK;
}

void Object_Manager::Priority_Update(_float fTimeDelta)
{
	for (uint32_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Priority_Update(fTimeDelta);
		}
	}
}

void Object_Manager::Update(_float fTimeDelta)
{
	for (uint32_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Update(fTimeDelta);
		}
	}
}

void Object_Manager::Late_Update(_float fTimeDelta)
{
	for (uint32_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{

			Pair.second->Late_Update(fTimeDelta);
		}
	}
}

void Object_Manager::Clear(uint32_t iClearLevelIndex)
{
	if (iClearLevelIndex >= m_iNumLevels ||
		nullptr == m_pLayers)
		return;

	for (auto& Pair : m_pLayers[iClearLevelIndex])
	{
		Pair.second.reset();
	}
	m_pLayers[iClearLevelIndex].clear();

}
HRESULT Object_Manager::Add_GameObject_toLayer(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag, uint32_t iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	if (nullptr == m_pLayers ||
		iLayerLevelIndex >= m_iNumLevels)
		return E_FAIL;



	auto	pGameObject = dynamic_pointer_cast<GameObject>(CGameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
		return E_FAIL;
	

	auto		pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
 		auto	pNewLayer = Layer::Create();
		if (nullptr == pNewLayer)
			return E_FAIL;

		if (FAILED(pNewLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, std::move(pNewLayer));
	}

	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

Layer* Object_Manager::Find_Layer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag)
{
	if (iLayerLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLayerLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLayerLevelIndex].end())
		return nullptr;

	return iter->second.get();
}

shared_ptr<GameObject> Object_Manager::Find_Object(uint32_t iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strObjectTag)
{
	auto pLayer =Find_Layer(iLayerLevelIndex, strLayerTag);

	if (pLayer == nullptr)
		return nullptr;

	for (auto obj : Find_Layer(iLayerLevelIndex, strLayerTag)->Get_GameObjects()) {
		if (obj->GetObjectINFO().m_strName == strObjectTag) {
			return obj;
		}
	}
	return nullptr;
}

map<const _wstring, unique_ptr<Layer>>& Object_Manager::Find_Layer_Lists(uint32_t iLayerLevelIndex)
{
	return m_pLayers.get()[iLayerLevelIndex];
}






unique_ptr<Object_Manager> Object_Manager::Create(uint32_t iNumLevels)
{
	auto		pInstance = unique_ptr<Object_Manager>(new Object_Manager());

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : Object_Manager");
		return nullptr;
	}

	return pInstance;
}
