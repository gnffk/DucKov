#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
class Graphic_Device final
{
private:
	Graphic_Device();

public:
	~Graphic_Device();

public:
	HRESULT Initialize(HWND hWnd, WINMODE eWinMode, int32_t iWinSizeX, int32_t iWinSizeY,
		ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutContext);

	HRESULT Clear_BackBuffer_View(const _float4* pClearColor);

	HRESULT Clear_DepthStencil_View();

	HRESULT Present();

	void Shutdown();

private:
	ComPtr<ID3D11Device>		m_pDevice{ nullptr };

	ComPtr<ID3D11DeviceContext> m_pDeviceContext{ nullptr };

	ComPtr<IDXGISwapChain>		m_pSwapChain{ nullptr };

	ComPtr<ID3D11RenderTargetView> m_pBackBufferRTV = { nullptr };

	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = { nullptr };

private:
	HRESULT Ready_SwapChain(HWND hWnd, WINMODE isWindowed, int32_t iWinCX, int32_t iWincY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilView(int32_t iWinCX, int32_t iWinCY);

public:
	static unique_ptr<Graphic_Device> Create(HWND hWnd, WINMODE isWindowed, int32_t iWinCX, int32_t iWinCY,
		ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutContext);
};

NS_END