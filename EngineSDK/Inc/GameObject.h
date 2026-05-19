#pragma once
#include "Transform.h"
#include "GameInstance.h"
NS_BEGIN(Engine)

class ENGINE_DLL GameObject abstract : public Prototype
{
public:
	typedef struct tagGameObjectDesc : public Transform::TRANSFORM_DESC
	{
		uint32_t	  ObjectType;
		uint32_t	  LevelIndex;
		_bool		  m_bCollider = true;
		wstring		  m_strName;
		wstring		  m_strPrototypeObjectName;
		wstring		  m_strPrototypeBaseName;
		uint32_t	  pCameraType;
		_bool		  ContainerObject{false};
	}GAMEOBJECT_DESC;
protected:
	GameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	GameObject(const GameObject& Prototype);
public:
	virtual ~GameObject();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Set_Dead() { m_bDead = true; }
	_bool Get_Dead() { return m_bDead; }
public:
	Transform* GetTransform() { return m_pTransformCom.get(); }
	GAMEOBJECT_DESC& GetObjectINFO() { return Object_INFO; }
	map<const _wstring, shared_ptr<class Component>>& GetComponents();


protected:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	shared_ptr<class Transform>	m_pTransformCom = { nullptr };

	


protected:
	// clone 한 컴포넌트들을 보관하는 곳
	map<const _wstring, shared_ptr<class Component>>		m_Components;
	GAMEOBJECT_DESC Object_INFO;

	_bool			m_bDead = false;

protected:
	HRESULT Add_Component(const _wstring& strComponentTag, shared_ptr<Component> pComponent);

	HRESULT Clear_Compnent();

	template<typename T>
	HRESULT Add_Component(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag,
		const _wstring& strComponentTag, shared_ptr<T>& pOut, void* pArg = nullptr)
	{
		if (nullptr != Find_Component(strComponentTag))
			return E_FAIL;

		auto	pComponent = dynamic_pointer_cast<Component>(CGameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));
		if (nullptr == pComponent)
			return E_FAIL;

		m_Components.emplace(strComponentTag, pComponent);

		pOut = dynamic_pointer_cast<T>(pComponent);

		return S_OK;
	}


	shared_ptr<class Component> Find_Component(const _wstring& strComponentTag);

	
public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;

};

NS_END
