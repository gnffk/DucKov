#include "ContainerObject.h"
#include "PartObject.h"

#include "GameInstance.h"

ContainerObject::ContainerObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }
{
}

ContainerObject::ContainerObject(const ContainerObject& Prototype)
	: GameObject{ Prototype }
{
}

ContainerObject::~ContainerObject()
{
}

HRESULT ContainerObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT ContainerObject::Initialize(void* pArg)
{
	auto        pDesc = static_cast<CONTAINEROBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void ContainerObject::Priority_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Update(fTimeDelta);
	}
}

void ContainerObject::Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Update(fTimeDelta);
	}
}

void ContainerObject::Late_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Update(fTimeDelta);
	}
}

HRESULT ContainerObject::Render()
{
	return S_OK;
}

PartObject* ContainerObject::Find_PartObject(const _wstring& strPartTag)
{
	auto	iter = m_PartObjects.find(strPartTag);
	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second.get();
}

HRESULT ContainerObject::Add_PartObject(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strPartTag, void* pArg)
{
	if (nullptr != Find_PartObject(strPartTag))
		return E_FAIL;

	auto	pPartObject = dynamic_pointer_cast<PartObject>(CGameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, pPartObject);

	return S_OK;
}
