#include "MainMenu_UI.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"
#include "Level_Loading.h"

#include <fstream>
#include <sstream>
#include <algorithm>

MainMenu_UI::MainMenu_UI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : UIObject{ pDevice, pContext }
{
}

MainMenu_UI::MainMenu_UI(const MainMenu_UI& Prototype)
    : UIObject{ Prototype }
{
}

MainMenu_UI::~MainMenu_UI()
{
}

HRESULT MainMenu_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT MainMenu_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_UI()))
        return E_FAIL;

    return S_OK;
}

void MainMenu_UI::Priority_Update(_float fTimeDelta)
{
}

void MainMenu_UI::Update(_float fTimeDelta)
{
    m_vMouseUIPos = Get_MouseUIPos();

    Update_Button();
    Update_SettingPanel();

//#ifdef _DEBUG
//    GUI_MainMenuUI();
//#endif
}

void MainMenu_UI::Late_Update(_float fTimeDelta)
{
    CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(MainMenu_UI));
}

HRESULT MainMenu_UI::Render()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

    // Rect 먼저
    for (auto& Pair : m_UIRects)
    {
        if (FAILED(Render_UIRect_ByKey(Pair.first)))
            return E_FAIL;
    }

    // Text 마지막
    for (auto& Pair : m_UITexts)
    {
        UI_TEXT& Text = Pair.second;

        if (false == Text.bVisible)
            continue;

        Text.fScale = std::clamp(Text.fScale, 0.01f, 10.f);

        Text.vColor.x = std::clamp(Text.vColor.x, 0.f, 1.f);
        Text.vColor.y = std::clamp(Text.vColor.y, 0.f, 1.f);
        Text.vColor.z = std::clamp(Text.vColor.z, 0.f, 1.f);
        Text.vColor.w = std::clamp(Text.vColor.w, 0.f, 1.f);

        CGameInstance::Get().Draw_Text(
            Text.strFontTag,
            Text.strText.c_str(),
            Text.vPos,
            Text.fScale,
            XMLoadFloat4(&Text.vColor),
            0.f,
            _float2(0.f, 0.f));
    }

    return S_OK;
}

HRESULT MainMenu_UI::Ready_Components()
{
    if (FAILED(__super::Add_Component(
        CGameInstance::Get().Get_Level(),
        TEXT("Prototype_Com_Shader_VtxPosTex"),
        TEXT("Com_Shader"),
        m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(
        CGameInstance::Get().Get_Level(),
        TEXT("Prototype_Com_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"),
        m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT MainMenu_UI::Ready_UI()
{
    m_UIRects.clear();
    m_UITexts.clear();

    // 배경 이미지
    if (FAILED(Add_UIRect(
        TEXT("BackGround"),
        TEXT("BackGround"),
        TEXT("Prototype_Com_Texture_MainMenu_BackGround"),
        { 640.f, 360.f },
        { 1280.f, 720.f },
        0.0f)))
        return E_FAIL;

    // 로고 이미지
    if (FAILED(Add_UIRect(
        TEXT("Logo"),
        TEXT("Logo"),
        TEXT("Prototype_Com_Texture_MainMenu_Logo"),
        { 640.f, 150.f },
        { 560.f, 160.f },
        0.0f)))
        return E_FAIL;

    // 시작 버튼 박스
    if (FAILED(Add_UIRect(
        TEXT("StartButton"),
        TEXT("StartButton"),
        TEXT("Prototype_Com_Texture_UI_BaseRectCustom"),
        { 640.f, 370.f },
        { 320.f, 80.f },
        0.0f)))
        return E_FAIL;

    // 설정 버튼 박스
    if (FAILED(Add_UIRect(
        TEXT("SettingButton"),
        TEXT("SettingButton"),
        TEXT("Prototype_Com_Texture_UI_BaseRectCustom"),
        { 640.f, 470.f },
        { 320.f, 80.f },
        0.0f)))
        return E_FAIL;

    // 나가기 버튼 박스
    if (FAILED(Add_UIRect(
        TEXT("ExitButton"),
        TEXT("ExitButton"),
        TEXT("Prototype_Com_Texture_UI_BaseRectCustom"),
        { 640.f, 570.f },
        { 320.f, 80.f },
        0.0f)))
        return E_FAIL;

    // 설정 패널
    if (FAILED(Add_UIRect(
        TEXT("SettingPanel"),
        TEXT("SettingPanel"),
        TEXT("Prototype_Com_Texture_UI_BaseRectCustom1"),
        { 640.f, 360.f },
        { 520.f, 420.f },
        0.0f)))
        return E_FAIL;

    m_UIRects[TEXT("SettingPanel")].bVisible = false;
    m_UIRects[TEXT("SettingPanel")].fAlpha = 0.85f;

    // 버튼 글자
    if (FAILED(Add_UIText(
        TEXT("Text_Start"),
        TEXT("Font_Default"),
        TEXT("시작"),
        { 600.f, 345.f },
        0.9f,
        { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;

    if (FAILED(Add_UIText(
        TEXT("Text_Setting"),
        TEXT("Font_Default"),
        TEXT("설정"),
        { 600.f, 445.f },
        0.9f,
        { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;

    if (FAILED(Add_UIText(
        TEXT("Text_Exit"),
        TEXT("Font_Default"),
        TEXT("나가기"),
        { 585.f, 545.f },
        0.9f,
        { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;

    // 설정창 글자
    if (FAILED(Add_UIText(
        TEXT("Text_SettingTitle"),
        TEXT("Font_Default"),
        TEXT("설정"),
        { 600.f, 180.f },
        0.9f,
        { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;

    if (FAILED(Add_UIText(
        TEXT("Text_SettingDesc"),
        TEXT("Font_Default"),
        TEXT("아직 설정 항목 없음"),
        { 505.f, 300.f },
        0.6f,
        { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;

    m_UITexts[TEXT("Text_SettingTitle")].bVisible = false;
    m_UITexts[TEXT("Text_SettingDesc")].bVisible = false;

    Load_UIRects(TEXT("../../Resources/Data/UI/MainMenuUI.txt"));
    Load_UITexts(TEXT("../../Resources/Data/UI/MainMenuUI_Text.txt"));

    return S_OK;
}

HRESULT MainMenu_UI::Add_UIRect(
    const wstring& UIName,
    const wstring& strName,
    const wstring& strTextureTag,
    const _float2& vPos,
    const _float2& vSize,
    _float fDepth)
{
    UI_RECT UI{};

    UI.strName = strName;
    UI.strTextureTag = strTextureTag;
    UI.vPos = vPos;
    UI.vSize = vSize;
    UI.fDepth = fDepth;
    UI.iTextureIndex = 0;
    UI.bVisible = true;
    UI.fAlpha = 1.f;
    UI.vColor = { 1.f, 1.f, 1.f, 1.f };

    if (FAILED(__super::Add_Component(
        CGameInstance::Get().Get_Level(),
        strTextureTag.c_str(),
        UI.strName,
        UI.pTexture)))
        return E_FAIL;

    m_UIRects[UIName] = UI;

    return S_OK;
}

HRESULT MainMenu_UI::Add_UIText( const wstring& TextName, const wstring& FontTag, const wstring& Text, const _float2& vPos, _float fScale,const _float4& vColor)
{
    UI_TEXT Desc{};

    Desc.strFontTag = FontTag;
    Desc.strText = Text;
    Desc.vPos = vPos;
    Desc.fScale = fScale;
    Desc.vColor = vColor;
    Desc.bVisible = true;

    m_UITexts[TextName] = Desc;

    return S_OK;
}

HRESULT MainMenu_UI::Render_UIRect(UI_RECT& UI)
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float fX = UI.vPos.x - vViewportSize.x * 0.5f;
    _float fY = -UI.vPos.y + vViewportSize.y * 0.5f;

    m_pTransformCom->Set_Scale(UI.vSize.x, UI.vSize.y, 1.f);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(fX, fY, UI.fDepth, 1.f));

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(UI.pTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture", UI.iTextureIndex)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT MainMenu_UI::Render_UIRect_ByKey(const wstring& strKey)
{
    auto iter = m_UIRects.find(strKey);

    if (iter == m_UIRects.end())
        return S_OK;

    UI_RECT& UI = iter->second;

    if (false == UI.bVisible)
        return S_OK;

    if (nullptr == UI.pTexture)
        return S_OK;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &UI.fAlpha, sizeof(float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &UI.vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(Render_UIRect(UI)))
        return E_FAIL;

    return S_OK;
}

void MainMenu_UI::Update_Button()
{
    m_iHoverButton = -1;

    auto iterStart = m_UIRects.find(TEXT("StartButton"));
    auto iterSetting = m_UIRects.find(TEXT("SettingButton"));
    auto iterExit = m_UIRects.find(TEXT("ExitButton"));

    if (iterStart != m_UIRects.end())
    {
        if (Is_PointInRect(m_vMouseUIPos, iterStart->second))
            m_iHoverButton = 0;
    }

    if (iterSetting != m_UIRects.end())
    {
        if (Is_PointInRect(m_vMouseUIPos, iterSetting->second))
            m_iHoverButton = 1;
    }

    if (iterExit != m_UIRects.end())
    {
        if (Is_PointInRect(m_vMouseUIPos, iterExit->second))
            m_iHoverButton = 2;
    }

    if (iterStart != m_UIRects.end())
        iterStart->second.fAlpha = (m_iHoverButton == 0) ? 1.f : 0.65f;

    if (iterSetting != m_UIRects.end())
        iterSetting->second.fAlpha = (m_iHoverButton == 1) ? 1.f : 0.65f;

    if (iterExit != m_UIRects.end())
        iterExit->second.fAlpha = (m_iHoverButton == 2) ? 1.f : 0.65f;

    auto SetTextColor = [&](const wstring& strKey, _bool bHover)
        {
            auto iter = m_UITexts.find(strKey);

            if (iter == m_UITexts.end())
                return;

            if (bHover)
            {
                iter->second.vColor = { 1.f, 0.85f, 0.2f, 1.f };
                iter->second.fScale = 1.0f;
            }
            else
            {
                iter->second.vColor = { 1.f, 1.f, 1.f, 1.f };
                iter->second.fScale = 0.9f;
            }
        };

    SetTextColor(TEXT("Text_Start"), m_iHoverButton == 0);
    SetTextColor(TEXT("Text_Setting"), m_iHoverButton == 1);
    SetTextColor(TEXT("Text_Exit"), m_iHoverButton == 2);

    if (false == CGameInstance::Get().Mouse_Down(MOUSEKEYSTATE::DIM_LB))
        return;

    if (m_iHoverButton == 0)
    {
        Click_Start();
        return;
    }

    if (m_iHoverButton == 1)
    {
        Click_Setting();
        return;
    }

    if (m_iHoverButton == 2)
    {
        Click_Exit();
        return;
    }
}

void MainMenu_UI::Update_SettingPanel()
{
    auto iterPanel = m_UIRects.find(TEXT("SettingPanel"));
    auto iterTitle = m_UITexts.find(TEXT("Text_SettingTitle"));
    auto iterDesc = m_UITexts.find(TEXT("Text_SettingDesc"));

    if (iterPanel != m_UIRects.end())
        iterPanel->second.bVisible = m_bSettingOpen;

    if (iterTitle != m_UITexts.end())
        iterTitle->second.bVisible = m_bSettingOpen;

    if (iterDesc != m_UITexts.end())
        iterDesc->second.bVisible = m_bSettingOpen;
}

void MainMenu_UI::Click_Start()
{
    if (FAILED(CGameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING),Level_Loading::Create(m_pDevice, m_pContext, LEVEL::HOME)))) {

        return;
    }
}

void MainMenu_UI::Click_Setting()
{
    m_bSettingOpen = !m_bSettingOpen;
}

void MainMenu_UI::Click_Exit()
{
    PostQuitMessage(0);
}

_float2 MainMenu_UI::Get_MouseUIPos()
{
    POINT pt{};
    GetCursorPos(&pt);
    ScreenToClient(g_hWnd, &pt);

    return _float2(
        static_cast<float>(pt.x),
        static_cast<float>(pt.y)
    );
}

_bool MainMenu_UI::Is_PointInRect(const _float2& vPoint, const UI_RECT& Rect)
{
    float fHalfX = Rect.vSize.x * 0.5f;
    float fHalfY = Rect.vSize.y * 0.5f;

    if (vPoint.x < Rect.vPos.x - fHalfX)
        return false;

    if (vPoint.x > Rect.vPos.x + fHalfX)
        return false;

    if (vPoint.y < Rect.vPos.y - fHalfY)
        return false;

    if (vPoint.y > Rect.vPos.y + fHalfY)
        return false;

    return true;
}

HRESULT MainMenu_UI::Save_UIRects(const wstring& strFilePath)
{
    ofstream ofs(strFilePath);

    if (!ofs.is_open())
        return E_FAIL;

    for (auto& Pair : m_UIRects)
    {
        const wstring& strName = Pair.first;
        const UI_RECT& UI = Pair.second;

        ofs
            << CGameInstance::Get().WStringToString(strName) << "|"
            << CGameInstance::Get().WStringToString(UI.strTextureTag) << "|"
            << UI.vPos.x << "|"
            << UI.vPos.y << "|"
            << UI.vSize.x << "|"
            << UI.vSize.y << "|"
            << UI.fDepth << "|"
            << static_cast<int>(UI.bVisible) << "|"
            << UI.iTextureIndex << "|"
            << UI.fAlpha << "|"
            << UI.vColor.x << "|"
            << UI.vColor.y << "|"
            << UI.vColor.z << "|"
            << UI.vColor.w
            << endl;
    }

    ofs.close();

    return S_OK;
}

HRESULT MainMenu_UI::Load_UIRects(const wstring& strFilePath)
{
    ifstream ifs(strFilePath);

    if (!ifs.is_open())
        return S_OK;

    string strLine;

    while (getline(ifs, strLine))
    {
        if (strLine.empty())
            continue;

        stringstream ss(strLine);
        string token;
        vector<string> Tokens;

        while (getline(ss, token, '|'))
            Tokens.push_back(token);

        if (Tokens.size() < 9)
            continue;

        wstring strName =
            CGameInstance::Get().StringToWString(Tokens[0]);

        auto iter = m_UIRects.find(strName);

        if (iter == m_UIRects.end())
            continue;

        UI_RECT& UI = iter->second;

        try
        {
            UI.strTextureTag =
                CGameInstance::Get().StringToWString(Tokens[1]);

            UI.vPos = { stof(Tokens[2]), stof(Tokens[3]) };
            UI.vSize = { stof(Tokens[4]), stof(Tokens[5]) };
            UI.fDepth = stof(Tokens[6]);
            UI.bVisible = stoi(Tokens[7]) != 0;
            UI.iTextureIndex = static_cast<int32_t>(stoi(Tokens[8]));

            if (Tokens.size() >= 10)
                UI.fAlpha = stof(Tokens[9]);
            else
                UI.fAlpha = 1.f;

            if (Tokens.size() >= 14)
            {
                UI.vColor =
                {
                    stof(Tokens[10]),
                    stof(Tokens[11]),
                    stof(Tokens[12]),
                    stof(Tokens[13])
                };
            }
            else
            {
                UI.vColor = { 1.f, 1.f, 1.f, 1.f };
            }
        }
        catch (...)
        {
            continue;
        }
    }

    ifs.close();

    return S_OK;
}

HRESULT MainMenu_UI::Save_UITexts(const wstring& strFilePath)
{
    ofstream ofs(strFilePath);

    if (!ofs.is_open())
        return E_FAIL;

    for (auto& Pair : m_UITexts)
    {
        const wstring& TextName = Pair.first;
        const UI_TEXT& Text = Pair.second;

        ofs
            << CGameInstance::Get().WStringToString(TextName) << "|"
            << CGameInstance::Get().WStringToString(Text.strFontTag) << "|"
            << CGameInstance::Get().WStringToString(Text.strText) << "|"
            << Text.vPos.x << "|"
            << Text.vPos.y << "|"
            << Text.fScale << "|"
            << static_cast<int>(Text.bVisible) << "|"
            << Text.vColor.x << "|"
            << Text.vColor.y << "|"
            << Text.vColor.z << "|"
            << Text.vColor.w
            << endl;
    }

    ofs.close();

    return S_OK;
}

HRESULT MainMenu_UI::Load_UITexts(const wstring& strFilePath)
{
    ifstream ifs(strFilePath);

    if (!ifs.is_open())
        return S_OK;

    string strLine;

    while (getline(ifs, strLine))
    {
        if (strLine.empty())
            continue;

        stringstream ss(strLine);
        string token;
        vector<string> Tokens;

        while (getline(ss, token, '|'))
            Tokens.push_back(token);

        if (Tokens.size() < 6)
            continue;

        wstring TextName =
            CGameInstance::Get().StringToWString(Tokens[0]);

        auto iter = m_UITexts.find(TextName);

        if (iter == m_UITexts.end())
            continue;

        UI_TEXT& Text = iter->second;

        try
        {
            Text.strFontTag = CGameInstance::Get().StringToWString(Tokens[1]);
            Text.strText = CGameInstance::Get().StringToWString(Tokens[2]);

            Text.vPos =
            {
                stof(Tokens[3]),
                stof(Tokens[4])
            };

            if (Tokens.size() >= 11)
            {
                Text.fScale = stof(Tokens[5]);
                Text.bVisible = stoi(Tokens[6]) != 0;

                Text.vColor =
                {
                    stof(Tokens[7]),
                    stof(Tokens[8]),
                    stof(Tokens[9]),
                    stof(Tokens[10])
                };
            }
            else
            {
                Text.fScale = 1.f;
                Text.bVisible = stoi(Tokens[5]) != 0;
                Text.vColor = { 1.f, 1.f, 1.f, 1.f };
            }

            Text.fScale = std::clamp(Text.fScale, 0.01f, 10.f);

            Text.vColor.x = std::clamp(Text.vColor.x, 0.f, 1.f);
            Text.vColor.y = std::clamp(Text.vColor.y, 0.f, 1.f);
            Text.vColor.z = std::clamp(Text.vColor.z, 0.f, 1.f);
            Text.vColor.w = std::clamp(Text.vColor.w, 0.f, 1.f);
        }
        catch (...)
        {
            continue;
        }
    }

    ifs.close();

    return S_OK;
}

#ifdef _DEBUG

void MainMenu_UI::GUI_MainMenuUI()
{
    string strWindowName = "MainMenuUI Editor##";
    strWindowName += to_string(reinterpret_cast<size_t>(this));

    if (ImGui::Begin(strWindowName.c_str()))
    {
        if (ImGui::Button("Save UI"))
        {
            Save_UIRects(TEXT("../../Resources/Data/UI/MainMenuUI.txt"));
            Save_UITexts(TEXT("../../Resources/Data/UI/MainMenuUI_Text.txt"));
        }

        ImGui::SameLine();

        if (ImGui::Button("Load UI"))
        {
            Load_UIRects(TEXT("../../Resources/Data/UI/MainMenuUI.txt"));
            Load_UITexts(TEXT("../../Resources/Data/UI/MainMenuUI_Text.txt"));
        }

        ImGui::Separator();

        ImGui::Text("UI Rect Count : %d", static_cast<int>(m_UIRects.size()));

        int iRect = 0;

        for (auto& Pair : m_UIRects)
        {
            const wstring& UIName = Pair.first;
            UI_RECT& UI = Pair.second;

            string strName = CGameInstance::Get().WStringToString(UIName);

            string strHeader = "UI Rect ";
            strHeader += to_string(iRect);
            strHeader += " : ";
            strHeader += strName;

            ImGui::PushID(("Rect_" + strName).c_str());

            if (ImGui::CollapsingHeader(strHeader.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("Visible", reinterpret_cast<bool*>(&UI.bVisible));

                ImGui::DragFloat2("Position", reinterpret_cast<float*>(&UI.vPos), 1.f, -5000.f, 5000.f);
                ImGui::DragFloat2("Size", reinterpret_cast<float*>(&UI.vSize), 1.f, 0.f, 5000.f);
                ImGui::DragFloat("Depth", &UI.fDepth, 0.001f, 0.f, 1.f);
                ImGui::SliderFloat("Alpha", &UI.fAlpha, 0.f, 1.f);
                ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&UI.vColor));

                int iTexIndex = static_cast<int>(UI.iTextureIndex);

                if (ImGui::InputInt("Texture Index", &iTexIndex))
                {
                    if (iTexIndex < 0)
                        iTexIndex = 0;

                    UI.iTextureIndex = static_cast<int32_t>(iTexIndex);
                }

                string strTextureTag = CGameInstance::Get().WStringToString(UI.strTextureTag);

                ImGui::Text("Texture Prototype");
                ImGui::Text("%s", strTextureTag.c_str());
            }

            ImGui::PopID();
            ImGui::Separator();

            ++iRect;
        }

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("Font Text Count : %d", static_cast<int>(m_UITexts.size()));

        int iText = 0;

        for (auto& Pair : m_UITexts)
        {
            const wstring& TextName = Pair.first;
            UI_TEXT& Text = Pair.second;

            string strName = CGameInstance::Get().WStringToString(TextName);

            string strHeader = "Font Text ";
            strHeader += to_string(iText);
            strHeader += " : ";
            strHeader += strName;

            ImGui::PushID(("Text_" + strName).c_str());

            if (ImGui::CollapsingHeader(strHeader.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("Visible", reinterpret_cast<bool*>(&Text.bVisible));

                ImGui::DragFloat2("Text Position", reinterpret_cast<float*>(&Text.vPos), 1.f, -5000.f, 5000.f);
                ImGui::DragFloat("Text Scale", &Text.fScale, 0.01f, 0.01f, 10.f);
                ImGui::ColorEdit4("Text Color", reinterpret_cast<float*>(&Text.vColor));

                string strFontTag = CGameInstance::Get().WStringToString(Text.strFontTag);
                string strText = CGameInstance::Get().WStringToString(Text.strText);

                char szFontTag[256]{};
                char szText[256]{};

                strcpy_s(szFontTag, strFontTag.c_str());
                strcpy_s(szText, strText.c_str());

                if (ImGui::InputText("Font Tag", szFontTag, sizeof(szFontTag)))
                {
                    Text.strFontTag =
                        CGameInstance::Get().StringToWString(szFontTag);
                }

                if (ImGui::InputText("Text", szText, sizeof(szText)))
                {
                    Text.strText =
                        CGameInstance::Get().StringToWString(szText);
                }
            }

            ImGui::PopID();
            ImGui::Separator();

            ++iText;
        }
    }

    ImGui::End();
}

#endif

unique_ptr<MainMenu_UI> MainMenu_UI::Create(
    ComPtr<ID3D11Device> pDevice,
    ComPtr<ID3D11DeviceContext> pContext)
{
    unique_ptr<MainMenu_UI> pInstance =
        unique_ptr<MainMenu_UI>(new MainMenu_UI(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : MainMenu_UI");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> MainMenu_UI::Clone(void* pArg)
{
    shared_ptr<MainMenu_UI> pInstance =
        shared_ptr<MainMenu_UI>(new MainMenu_UI(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : MainMenu_UI");
        return nullptr;
    }

    return pInstance;
}