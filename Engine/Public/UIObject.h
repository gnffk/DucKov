#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL UIObject abstract : public GameObject
{
public:
	typedef struct tagUIObjectDesc : public GameObject::GAMEOBJECT_DESC
	{
		_float			fX, fY, fSizeX, fSizeY;
	}UIOBJECT_DESC;

protected:
	UIObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~UIObject();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_float				m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float4x4			m_ViewMatrix{}, m_ProjMatrix{};

protected:
	void Update_Transform();



};

NS_END