#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class Light final
{
private:
	Light(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~Light() = default;

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Render(shared_ptr<class Shader> pShader, shared_ptr<class VIBuffer_Rect> pVIBuffer);


private:
	ComPtr<ID3D11Device>		m_pDevice = { };
	ComPtr<ID3D11DeviceContext>	m_pContext = { };

	LIGHT_DESC					m_LightDesc{};

public:
	static shared_ptr<Light> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const LIGHT_DESC& LightDesc);

};

NS_END