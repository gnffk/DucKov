
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

#ifdef _DEBUG
public:
	void ImGui_Render();
	void Init_Debug_LightDesc(LIGHT_DESC& Desc);
	void Draw_LightDesc_Imgui(LIGHT_DESC& Desc);

private:
	int         m_iSelectedLight = -1;
	LIGHT_DESC  m_DebugCreateLightDesc{};
	bool        m_bDebugCreateLightInit = false;
#endif

private:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };

	vector<shared_ptr<class Light>> m_Lights;


public:
	static unique_ptr<Light_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);



};

NS_END