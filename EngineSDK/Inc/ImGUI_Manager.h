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
	HRESULT Initialize(HWND hWnd, ComPtr<ID3D11Device> p_Device, ComPtr<ID3D11DeviceContext> p_DeviceContext, ComPtr<ID3D11RenderTargetView> p_MainRTV, ComPtr<ID3D11DepthStencilView> p_MainDSV);

	void Update_Imgui(_float fTimeDelta);
	void Render_Imgui();

	bool ImGui_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);




private:
	
	ComPtr<ID3D11Device> m_pDevice;
	ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	ComPtr<ID3D11DepthStencilState> pDepthDisabledState;

	ComPtr<ID3D11RenderTargetView> m_pMainRTV;
	ComPtr<ID3D11DepthStencilView> m_pMainDSV;
	D3D11_VIEWPORT m_MainViewport{};
	HWND m_hWnd;

public:
	static unique_ptr<ImGUI_Manager> Create(HWND hWnd, ComPtr<ID3D11Device>& p_Device, ComPtr<ID3D11DeviceContext>& p_DeviceContext, ComPtr<ID3D11RenderTargetView>& p_MainRTV, ComPtr<ID3D11DepthStencilView>& p_MainDSV);

};

NS_END