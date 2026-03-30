#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ImGUI_Manager final
{
private:
	ImGUI_Manager() = default;

public:
	~ImGUI_Manager();


public:
	HRESULT Initialize(HWND hWnd, ComPtr<ID3D11Device>p_Device, ComPtr<ID3D11DeviceContext>p_DeviceContext);

	void Update_Imgui(_float fTimeDelta);
	void Render_Imgui();

	bool ImGui_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


private:
	ComPtr<ID3D11Device>			m_pDevice{ nullptr };
	ComPtr<ID3D11DeviceContext>		m_pDeviceContext{ nullptr };
	ComPtr<ID3D11DepthStencilState> pDepthDisabledState{ nullptr };


public:
	static unique_ptr<ImGUI_Manager> Create(HWND hWnd,ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutContext);
};

NS_END