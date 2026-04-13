#pragma once
#include "Transform.h"
NS_BEGIN(Engine)

class ENGINE_DLL GameObject abstract : public Prototype
{
public:
	typedef struct tagGameObjectDesc : public Transform::TRANSFORM_DESC
	{
		const _tchar* pGameObjectTag;
	}GAMEOBJECT_DESC;
protected:
	GameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	GameObject(const GameObject& Prototype);
public:
	virtual ~GameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	shared_ptr<class Transform>	m_pTransformCom = { nullptr };

protected:
	_tchar					m_szName[MAX_PATH] = {};


protected:
	map<const _wstring, shared_ptr<class Component>>		m_Components;
protected:
	HRESULT Add_Component(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag,
		const _wstring& strComponentTag, shared_ptr<Component>& pOut, void* pArg = nullptr);

	shared_ptr<class Component> Find_Component(const _wstring& strComponentTag);

public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;

};

NS_END
