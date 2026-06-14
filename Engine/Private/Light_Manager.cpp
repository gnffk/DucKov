#include "Light_Manager.h"
#include "Light.h"

Light_Manager::Light_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{

}

Light_Manager::~Light_Manager()
{
}

HRESULT Light_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
    auto pLight = Light::Create(m_pDevice, m_pContext, LightDesc);
    if (nullptr == pLight)
        return E_FAIL;

    m_Lights.push_back(pLight);

    return S_OK;
}

HRESULT Light_Manager::Render(shared_ptr<class Shader> pShader, shared_ptr<class VIBuffer_Rect> pVIBuffer)
{
    for (auto& pLight : m_Lights)
    {
        pLight->Render(pShader, pVIBuffer);
    }

#ifdef _DEBUG
    ImGui_Render();
#endif
    return S_OK;
}

unique_ptr<Light_Manager>  Light_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return  unique_ptr<Light_Manager>(new Light_Manager(pDevice, pContext));
}

#ifdef _DEBUG

void  Light_Manager::Init_Debug_LightDesc(LIGHT_DESC& Desc)
{
    Desc = {};

    // 네 enum 이름에 맞게 사용
    Desc.eType = LIGHT::END;

    Desc.vPosition = XMFLOAT4(0.f, 5.f, 0.f, 1.f);
    Desc.vDirection = XMFLOAT4(0.f, -1.f, 0.f, 0.f);

    Desc.vDiffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
    Desc.vAmbient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
    Desc.vSpecular = XMFLOAT4(1.f, 1.f, 1.f, 1.f);

    Desc.fRange = 10.f;
}

void  Light_Manager::Draw_LightDesc_Imgui(LIGHT_DESC& Desc)
{
    int iType = static_cast<int>(Desc.eType);

    const char* LightTypes[] =
    {
        "Directional",
        "Point"
    };

    if (ImGui::Combo("Light Type", &iType, LightTypes, IM_ARRAYSIZE(LightTypes)))
    {
        Desc.eType = static_cast<LIGHT>(iType);
    }

    ImGui::Separator();

    if (Desc.eType == LIGHT::DIRECTIONAL)
    {
        ImGui::DragFloat4(
            "Direction",
            &Desc.vDirection.x,
            0.01f,
            -1.f,
            1.f
        );
    }

    if (Desc.eType == LIGHT::POINT)
    {
        ImGui::DragFloat4(
            "Position",
            &Desc.vPosition.x,
            0.1f
        );

        ImGui::DragFloat(
            "Range",
            &Desc.fRange,
            0.1f,
            0.1f,
            1000.f
        );
    }

    ImGui::Separator();

    ImGui::ColorEdit4("Diffuse", &Desc.vDiffuse.x);
    ImGui::ColorEdit4("Ambient", &Desc.vAmbient.x);
    ImGui::ColorEdit4("Specular", &Desc.vSpecular.x);
}

void Light_Manager::ImGui_Render()
{
    if (false == m_bDebugCreateLightInit)
    {
        Init_Debug_LightDesc(m_DebugCreateLightDesc);
        m_bDebugCreateLightInit = true;
    }

    if (!ImGui::Begin("Light Manager"))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Light Count : %d", static_cast<int>(m_Lights.size()));
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Create Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("CreateLight");

        Draw_LightDesc_Imgui(m_DebugCreateLightDesc);

        if (ImGui::Button("Add Light"))
        {
            if (SUCCEEDED(Add_Light(m_DebugCreateLightDesc)))
            {
                m_iSelectedLight = static_cast<int>(m_Lights.size()) - 1;
            }
        }

        ImGui::PopID();
    }
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Light List", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (int i = 0; i < static_cast<int>(m_Lights.size()); ++i)
        {
            char szName[64] = {};
            sprintf_s(szName, "Light %d", i);

            if (ImGui::Selectable(szName, m_iSelectedLight == i))
            {
                m_iSelectedLight = i;
            }
        }
    }

    ImGui::Separator();

    if (m_iSelectedLight >= 0 &&
        m_iSelectedLight < static_cast<int>(m_Lights.size()))
    {
        ImGui::Text("Selected Light : %d", m_iSelectedLight);
        ImGui::Separator();

        ImGui::PushID(m_iSelectedLight);

        LIGHT_DESC& LightDesc = m_Lights[m_iSelectedLight]->Get_LightDesc();

        Draw_LightDesc_Imgui(LightDesc);

        if (ImGui::Button("Delete Selected Light"))
        {
            m_Lights.erase(m_Lights.begin() + m_iSelectedLight);

            if (m_Lights.empty())
                m_iSelectedLight = -1;
            else if (m_iSelectedLight >= static_cast<int>(m_Lights.size()))
                m_iSelectedLight = static_cast<int>(m_Lights.size()) - 1;
        }

        ImGui::PopID();
    }

    ImGui::End();
}


#endif