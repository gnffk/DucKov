#pragma once
#include "Transform.h"
NS_BEGIN(Engine)

class ENGINE_DLL GameObject abstract : public Prototype
{
public:
	typedef struct tagGameObjectDesc : public Transform::TRANSFORM_DESC
	{
		wstring		  m_strName;
		uint32_t	  pCameraType;
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

public:
	Transform* GetTransform() { return m_pTransformCom.get(); }
	wstring GetObjectName() { return m_ObjectName; }
	map<const _wstring, shared_ptr<class Component>>& GetComponents() { return m_Components; }

protected:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	shared_ptr<class Transform>	m_pTransformCom = { nullptr };

	


protected:
	// clone 한 컴포넌트들을 보관하는 곳
	map<const _wstring, shared_ptr<class Component>>		m_Components;
	std::wstring m_ObjectName;


protected:
	HRESULT Add_Component(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag,
		const _wstring& strComponentTag, shared_ptr<Component>& pOut, void* pArg = nullptr);

	shared_ptr<class Component> Find_Component(const _wstring& strComponentTag);

	
public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;

};

NS_END
