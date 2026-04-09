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

HRESULT ImGUI_Manager::Initialize(HWND hWnd, ComPtr<ID3D11Device> p_Device, ComPtr<ID3D11DeviceContext> p_DeviceContext)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(p_Device.Get(), p_DeviceContext.Get());

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    ImFont* font = io.Fonts->AddFontFromFileTTF("../../Resources/Fonts/NanumGothic.ttf", 14.0f, NULL, io.Fonts->GetGlyphRangesKorean());

    if (font == nullptr) {
        // 만약 폰트 로드에 실패하면 기본 폰트라도 사용하게 설정
        io.Fonts->AddFontDefault();
    }
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    m_pDevice = p_Device;
    m_pDeviceContext = p_DeviceContext;
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};

    dsDesc.DepthEnable = FALSE;       
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; 
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    m_pDevice->CreateDepthStencilState(&dsDesc, &pDepthDisabledState);

 
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

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

}

bool ImGUI_Manager::ImGui_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!ImGui::GetCurrentContext()) return false;

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    return false;
}

unique_ptr<ImGUI_Manager> ImGUI_Manager::Create(HWND hWnd, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutContext)
{
    auto		pInstance = unique_ptr<ImGUI_Manager>(new ImGUI_Manager());

    if (FAILED(pInstance->Initialize(hWnd,  pOutDevice, pOutContext)))
    {
        MSG_BOX("Failed to Created : ImGUI_Manager");

    }

    return pInstance;
}

