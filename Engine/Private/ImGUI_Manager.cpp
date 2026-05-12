#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGUI_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
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

    // 멀티 뷰포트용 권장 스타일
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    m_hWnd = hWnd;
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





    RECT rc{};
    GetWindowRect(m_hWnd, &rc);

    float x = (float)rc.left;
    float y = (float)rc.top;

    ImGuizmo::SetRect(x, y, 1280, 720);

    Update_Guizmo();
}
void ImGUI_Manager::Render_Imgui()
{


    ComPtr<ID3D11DepthStencilState> pOldDepthState = nullptr;
    UINT oldStencilRef = 0;

    m_pDeviceContext->OMGetDepthStencilState(&pOldDepthState, &oldStencilRef);

    m_pDeviceContext->OMSetDepthStencilState(pDepthDisabledState.Get(), 0);
    
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        m_pDeviceContext->OMSetRenderTargets(1, m_pMainRTV.GetAddressOf(), m_pMainDSV.Get());
        m_pDeviceContext->RSSetViewports(1, &m_MainViewport);
    }

    m_pDeviceContext->OMSetDepthStencilState(pOldDepthState.Get(), oldStencilRef);

    if (pOldDepthState)
        pOldDepthState->Release();
}

void ImGUI_Manager::Update_Guizmo()
{
    
    if (!m_pSelectObject)
        return;

    _float4x4 view, proj;
    CGameInstance::Get().Get_MainCameraMatrix(view, proj);

    if (!CGameInstance::Get().Mouse_Pressing(MOUSEKEYSTATE::DIM_RB))
    {
        if (CGameInstance::Get().Key_Down(DIK_T))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (CGameInstance::Get().Key_Down(DIK_R))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (CGameInstance::Get().Key_Down(DIK_E))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
    }
    
    _float4x4 matrix = m_pSelectObject->GetTransform()->GetWorldMatrix();

    ImGui::Begin("Imguizmo");

    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }

    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&matrix), matrixTranslation, matrixRotation, matrixScale);

    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(&matrix));


    ImGui::End();

    ImGuizmo::Manipulate( reinterpret_cast<float*>(&view), reinterpret_cast<float*>(&proj), mCurrentGizmoOperation, mCurrentGizmoMode,reinterpret_cast<float*>(&matrix));
  
    m_pSelectObject->GetTransform()->Set_WorldMatrix(matrix);

}
void ImGUI_Manager::Clear() {
    m_pSelectObject = nullptr;
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

