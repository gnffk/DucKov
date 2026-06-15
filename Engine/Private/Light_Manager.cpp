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

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Light File", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputText("Json Path", m_szLightJsonPath, MAX_PATH);

        if (ImGui::Button("Save Lights"))
        {
            wchar_t wszPath[MAX_PATH] = {};
            MultiByteToWideChar(CP_ACP, 0, m_szLightJsonPath, -1, wszPath, MAX_PATH);

            Save_Lights_ToJson(wszPath);
        }

        ImGui::SameLine();

        if (ImGui::Button("Load Lights"))
        {
            wchar_t wszPath[MAX_PATH] = {};
            MultiByteToWideChar(CP_ACP, 0, m_szLightJsonPath, -1, wszPath, MAX_PATH);

            Load_Lights_FromJson(wszPath);
        }
    }

    ImGui::End();
}


#endif

static const char* LightType_ToString(LIGHT eType)
{
    switch (eType)
    {
    case LIGHT::DIRECTIONAL:
        return "Directional";

    case LIGHT::POINT:
        return "Point";

    default:
        return "Unknown";
    }
}

static LIGHT String_ToLightType(const std::string& strType)
{
    if (strType == "Directional")
        return LIGHT::DIRECTIONAL;

    if (strType == "Point")
        return LIGHT::POINT;

    return LIGHT::END;
}

static nlohmann::json Float4_ToJson(const XMFLOAT4& v)
{
    return nlohmann::json::array({ v.x, v.y, v.z, v.w });
}

static XMFLOAT4 Json_ToFloat4(const nlohmann::json& j)
{
    XMFLOAT4 v{};

    if (j.is_array() && j.size() >= 4)
    {
        v.x = j[0].get<float>();
        v.y = j[1].get<float>();
        v.z = j[2].get<float>();
        v.w = j[3].get<float>();
    }

    return v;
}

static nlohmann::json LightDesc_ToJson(const LIGHT_DESC& Desc)
{
    nlohmann::json j;

    j["type"] = LightType_ToString(Desc.eType);

    j["diffuse"] = Float4_ToJson(Desc.vDiffuse);
    j["ambient"] = Float4_ToJson(Desc.vAmbient);
    j["specular"] = Float4_ToJson(Desc.vSpecular);

    j["direction"] = Float4_ToJson(Desc.vDirection);
    j["position"] = Float4_ToJson(Desc.vPosition);

    j["range"] = Desc.fRange;

    return j;
}

static LIGHT_DESC Json_ToLightDesc(const nlohmann::json& j)
{
    LIGHT_DESC Desc{};

    Desc.eType = String_ToLightType(j.value("type", "Unknown"));

    Desc.vDiffuse = Json_ToFloat4(j.value("diffuse", nlohmann::json::array({ 1.f, 1.f, 1.f, 1.f })));
    Desc.vAmbient = Json_ToFloat4(j.value("ambient", nlohmann::json::array({ 0.2f, 0.2f, 0.2f, 1.f })));
    Desc.vSpecular = Json_ToFloat4(j.value("specular", nlohmann::json::array({ 1.f, 1.f, 1.f, 1.f })));

    Desc.vDirection = Json_ToFloat4(j.value("direction", nlohmann::json::array({ 0.f, -1.f, 0.f, 0.f })));
    Desc.vPosition = Json_ToFloat4(j.value("position", nlohmann::json::array({ 0.f, 5.f, 0.f, 1.f })));

    Desc.fRange = j.value("range", 10.f);

    return Desc;
}

HRESULT Light_Manager::Save_Lights_ToJson(const _wstring& strFilePath)
{
    nlohmann::json Root;
    Root["lights"] = nlohmann::json::array();

    for (auto& pLight : m_Lights)
    {
        if (nullptr == pLight)
            continue;

        const LIGHT_DESC& Desc = pLight->Get_LightDesc();

        Root["lights"].push_back(LightDesc_ToJson(Desc));
    }

    try
    {
        std::filesystem::path path(strFilePath);

        if (path.has_parent_path())
            std::filesystem::create_directories(path.parent_path());

        std::ofstream ofs(path);

        if (!ofs.is_open())
            return E_FAIL;

        ofs << Root.dump(4);
        ofs.close();
    }
    catch (...)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT Light_Manager::Load_Lights_FromJson(const _wstring& strFilePath)
{
    try
    {
        std::filesystem::path path(strFilePath);

        std::ifstream ifs(path);

        if (!ifs.is_open())
            return E_FAIL;

        nlohmann::json Root;
        ifs >> Root;
        ifs.close();

        if (!Root.contains("lights") || false == Root["lights"].is_array())
            return E_FAIL;

        m_Lights.clear();

        for (auto& LightJson : Root["lights"])
        {
            LIGHT_DESC Desc = Json_ToLightDesc(LightJson);

            if (Desc.eType == LIGHT::END)
                continue;

            if (FAILED(Add_Light(Desc)))
                return E_FAIL;
        }

#ifdef _DEBUG
        if (m_Lights.empty())
            m_iSelectedLight = -1;
        else
            m_iSelectedLight = 0;
#endif
    }
    catch (...)
    {
        return E_FAIL;
    }

    return S_OK;
}
