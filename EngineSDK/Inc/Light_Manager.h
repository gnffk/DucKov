
#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class Light_Manager final
{
private:
	Light_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~Light_Manager();

public:
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render(shared_ptr<class Shader> pShader, shared_ptr<class VIBuffer_Rect> pVIBuffer);

private:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };

	list<shared_ptr<class Light>>		m_Lights;


public:
	static unique_ptr<Light_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};

NS_END