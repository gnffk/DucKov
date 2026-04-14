#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGUI_Manager.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ImGUI_Manager::~ImGUI_Manager()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

HRESULT ImGUI_Manager::Initialize(HWND hWnd,
    ComPtr<ID3D11Device> p_Device,
    ComPtr<ID3D11DeviceContext> p_DeviceContext,
    ComPtr<ID3D11RenderTargetView> p_MainRTV,
    ComPtr<ID3D11DepthStencilView> p_MainDSV)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    // ¸ÖÆ¼ ºäÆ÷Æ®¿ë ±ÇÀå ½ºÅ¸ÀÏ
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(p_Device.Get(), p_DeviceContext.Get());

    ImFont* font = io.Fonts->AddFontFromFileTTF(
        "../../Resources/Fonts/NanumGothic.ttf",
        14.0f,
        nullptr,
        io.Fonts->GetGlyphRangesKorean());

    if (!font)
        io.Fonts->AddFontDefault();

    m_pDevice = p_Device;
    m_pDeviceContext = p_DeviceContext;
    m_pMainRTV = p_MainRTV;
    m_pMainDSV = p_MainDSV;

    UINT numViewports = 1;
    m_pDeviceContext->RSGetViewports(&numViewports, &m_MainViewport);

    D3D11_DEPTH_STENCIL_DESC dsDesc{};
    dsDesc.DepthEnable = FALSE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

    HRESULT hr = m_pDevice->CreateDepthStencilState(&dsDesc, &pDepthDisabledState);
    if (FAILED(hr))
        return hr;

    return S_OK;


}

void ImGUI_Manager::Update_Imgui(_float fTimeDelta)
{

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();


}

void ImGUI_Manager::Render_Imgui()
{

    m_pDeviceContext->OMSetDepthStencilState(pDepthDisabledState.Get(), 0);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        // ÇÙ½É: ¸ÞÀÎ ·»´õ Å¸°Ù/±íÀÌ ¹öÆÛ/ºäÆ÷Æ® º¹±¸
        m_pDeviceContext->OMSetRenderTargets(1, m_pMainRTV.GetAddressOf(), m_pMainDSV.Get());
        m_pDeviceContext->RSSetViewports(1, &m_MainViewport);
        m_pDeviceContext->OMSetDepthStencilState(pDepthDisabledState.Get(), 0);
    }
}

bool ImGUI_Manager::ImGui_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!ImGui::GetCurrentContext()) return false;

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    return false;
}

unique_ptr<ImGUI_Manager> ImGUI_Manager::Create(HWND hWnd, 
    ComPtr<ID3D11Device>& p_Device,
    ComPtr<ID3D11DeviceContext>& p_DeviceContext,
    ComPtr<ID3D11RenderTargetView>& p_MainRTV,
    ComPtr<ID3D11DepthStencilView>& p_MainDSV)
{
    auto		pInstance = unique_ptr<ImGUI_Manager>(new ImGUI_Manager());

    if (FAILED(pInstance->Initialize(hWnd, p_Device, p_DeviceContext, p_MainRTV, p_MainDSV)))
    {
        MSG_BOX("Failed to Created : ImGUI_Manager");

    }

    return pInstance;
}

