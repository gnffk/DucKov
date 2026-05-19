#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL ContainerObject abstract : public GameObject
{
public:
	typedef struct tagContainerObjectDesc : public GameObject::GAMEOBJECT_DESC
	{

	}CONTAINEROBJECT_DESC;
protected:
	ContainerObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	ContainerObject(const ContainerObject& Prototype);
public:
	virtual ~ContainerObject();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	map<const _wstring, shared_ptr<class PartObject>>		m_PartObjects;

protected:
	class PartObject* Find_PartObject(const _wstring& strPartTag);
	HRESULT Add_PartObject(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strPartTag, void* pArg = nullptr);

public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;
};

NS_END