#pragma once
#include "Component.h"

NS_BEGIN(Engine)

// Transform은 Clone하지 않고 바로 생성한다.

class ENGINE_DLL Transform final : public Component
{
public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = {};
		_float		fRotationPerSec = {};
	}TRANSFORM_DESC;

private:
	Transform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~Transform();

public:
	_vector Get_State(STATE eState) const {
		return XMLoadFloat4(reinterpret_cast<const _float4*>(&m_WorldMatrix.m[ETOUI(eState)][0]));
	}
	_float3 Get_Scaled();

	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ETOUI(eState)][0]), vState);
	}
	_float4x4 GetWorldMatrix(){return m_WorldMatrix;}
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);


public:
	void Set_Scale(_float fScaleX, _float fScaleY, _float fScaleZ);
	void Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Go_Static_Right(_float fTimeDelta);
	void Go_Static_Left(_float fTimeDelta);
	void Rotation(_fvector vAxis, _float fAngle);
	void Turn(_fvector vAxis, _float fTimeDelta);





	void LookAt(_fvector vAt);
	void Chase(_fvector vGoal, _float fTimeDelta, _float fLimit = 0.f);


private:
	_float			m_fSpeedPerSec = {};
	_float			m_fRotationPerSec = {};

	_float4x4		m_WorldMatrix = {};

public:
	static shared_ptr<Transform> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END