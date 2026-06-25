#pragma once

/* 그림자 연산에 이용할 광원의 정보를 카메라의 형태로 보관한다. */
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class Shadow final
{
public:
	typedef struct tagShadowLightDesc
	{
		_float4		vEye, vAt;
		_float		fFovy, fNear, fFar;
	}SHADOW_LIGHT_DESC;
private:
	Shadow();
public:
	~Shadow();

public:
	const _float4x4* Get_ShadowLightTransform(D3DTS eState);

public:
	HRESULT Add_Shadow_Light(const SHADOW_LIGHT_DESC& LightDesc);
public:
	HRESULT Set_ShadowLightDesc(const SHADOW_LIGHT_DESC& ShadowLightDesc);
private:
	_float4x4				m_LightMatrices[ETOUI(D3DTS::END)] = {};


	SHADOW_LIGHT_DESC m_ShadowLightDesc;

public:
	static unique_ptr<Shadow> Create();
};

NS_END