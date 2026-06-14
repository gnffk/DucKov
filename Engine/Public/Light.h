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
public:
	LIGHT_DESC& Get_LightDesc() { return m_LightDesc; }
	const LIGHT_DESC& Get_LightDesc() const { return m_LightDesc; }

	void Set_LightDesc(const LIGHT_DESC& LightDesc) { m_LightDesc = LightDesc; }


private:
	ComPtr<ID3D11Device>		m_pDevice = { };
	ComPtr<ID3D11DeviceContext>	m_pContext = { };

	LIGHT_DESC					m_LightDesc{};

public:
	static shared_ptr<Light> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const LIGHT_DESC& LightDesc);

};

NS_END