
#include "MapEditor.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Layer.h"
#include "BaseCollider.h"


MapEditor::MapEditor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

MapEditor::~MapEditor()
{
}

HRESULT MapEditor::Initialize()
{
	Ready_Layer_MapEditor(L"Base");

	CGameInstance::Get().Change_Camera(ETOUI(CAMERA::MAIN));

	return S_OK;
}

void MapEditor::Update(_float fTimeDelta)
{

	MousePicking();
}

HRESULT MapEditor::Ready_Layer_MapEditor(const _wstring& strLayerTag)
{
   GameObject::GAMEOBJECT_DESC descTerrian{};
   descTerrian.ObjectType = ETOUI(OBJECTTYPE::OBJECT_TERRIAN);
   descTerrian.m_strName =L"Terrian";
   descTerrian.m_strPrototypeObjectName = L"Prototype_GameObject_Terrain";
   descTerrian.m_strPrototypeBaseName =L"Terrian";
   descTerrian.pCameraType = ETOUI(CAMERA::NONE);
   descTerrian.fSpeedPerSec = 5.f;
   descTerrian.fRotationPerSec = 1.f;

    if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Terrain"),
        CGameInstance::Get().Get_Level(), strLayerTag, &descTerrian)))
        return E_FAIL;


    GameObject::GAMEOBJECT_DESC descSKY{};
    descSKY.ObjectType = ETOUI(OBJECTTYPE::OBJECT_SKYBOX);
    descSKY.m_strName = L"SkyBox";
    descSKY.m_strPrototypeObjectName = L"Prototype_GameObject_Sky";
    descSKY.m_strPrototypeBaseName = L"SkyBox";
    descSKY.pCameraType = ETOUI(CAMERA::NONE);
    descSKY.fSpeedPerSec = 5.f;
    descSKY.fRotationPerSec = 1.f;

    if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Sky"),
        CGameInstance::Get().Get_Level(), strLayerTag, &descSKY)))
        return E_FAIL;




    GameObject::GAMEOBJECT_DESC descMain_Camera{};

    descMain_Camera.ObjectType = ETOUI(OBJECTTYPE::OBJECT_CAMERA);
    descMain_Camera.m_strName = L"Main_Camera";
    descMain_Camera.m_strPrototypeObjectName = L"Prototype_GameObject_PerspectiveCamera";
    descMain_Camera.m_strPrototypeBaseName = L"Prototype_GameObject_PerspectiveCamera";
    descMain_Camera.pCameraType = ETOUI(CAMERA::MAIN);
    descMain_Camera.fSpeedPerSec = 10.f;
    descMain_Camera.fRotationPerSec = 0.1f;


    if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_PerspectiveCamera"), CGameInstance::Get().Get_Level(), strLayerTag, &descMain_Camera)))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_Camera(ETOUI(CAMERA::MAIN),
        dynamic_pointer_cast<Camera>(CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), strLayerTag, L"Main_Camera")))))
        return E_FAIL;



	return S_OK;
  
}


HRESULT MapEditor::Render()
{
	IMGUI_Render();

    CGameInstance::Get().Draw_Text(TEXT("Font_Default"), TEXT("Map Editor"), _float2(1100.f, 0.f));
	return S_OK;
}

unique_ptr<MapEditor> MapEditor::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<MapEditor>(new MapEditor(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
		MessageBox(g_hWnd, TEXT("Failed to Created : MapEditor"), nullptr, MB_OK);

	return pInstance;
}

void MapEditor::IMGUI_Render() {
#if _DEBUG

	IMGUI_OTHER_Render();

	IMGUI_Level_Render();
    IMGUI_SaveLoad_Render();
    IMGUI_MadeFunction();
    IMGUI_AddPlayer();
#endif

}
void MapEditor::IMGUI_Level_Render()
{// --------------------------------Levels -----------------------------------------------------
    ImGui::SetNextWindowBgAlpha(0.5f);

    ImGuiViewport* viewport =
        ImGui::GetMainViewport();

    float leftPanelWidth = 260.f;

    ImGui::SetNextWindowPos(
        ImVec2(
            viewport->WorkPos.x,
            viewport->WorkPos.y),
        ImGuiCond_Always);

    ImGui::SetNextWindowSize(
        ImVec2(
            leftPanelWidth,
            viewport->WorkSize.y),
        ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Level", nullptr, flags);

    // =====================================================
    // Layer Loop
    // =====================================================

    for (auto& layer :
        CGameInstance::Get().Find_Layer_Lists(
            CGameInstance::Get().Get_Level()))
    {
        std::string layerLabel =
            WStringToString(layer.first);

        // =================================================
        // Layer Tree
        // =================================================

        bool layerOpen =
            ImGui::TreeNode(layerLabel.c_str());

        // =================================================
        // 여기서 Layer 자체를 DropTarget로 사용
        // =================================================
        _bool OpenDataObject = false;
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("GAME_OBJECT"))
            {
                const char* objectType =
                    (const char*)payload->Data;

                string className =
                    objectType;

        

                // =========================================
                // 생성
                // =========================================
                
                if (className == "Monster")
                {
                   OpenDataObject = true;
                   ImGui::OpenPopup("Select Monster");

                }

                else if (className == "Obstacle")
                {
                    OpenDataObject = true;
                    ImGui::OpenPopup("Select Obstacle");

                }
             
            }
            ImGui::EndDragDropTarget();
        }

        // =================================================
        // Object List
        // =================================================

        if (layerOpen)
        {
            auto& gameObjects =
                layer.second->Get_GameObjects();

            for (auto& object : gameObjects)
            {
                if (object == nullptr)
                    continue;
                std::string objectName =
                    WStringToString(
                        object->GetObjectINFO().m_strName);

                ImGuiTreeNodeFlags objectFlags =
                    ImGuiTreeNodeFlags_OpenOnArrow |
                    ImGuiTreeNodeFlags_SpanAvailWidth |
                    (CGameInstance::Get().GetSelectObject()
                        == object.get()
                        ? ImGuiTreeNodeFlags_Selected
                        : 0);

                bool opened =
                    ImGui::TreeNodeEx(
                        (objectName
                            + "##"
                            + std::to_string(
                                (uintptr_t)object.get()))
                        .c_str(),
                        objectFlags);

                if (ImGui::IsItemClicked())
                {
                    CGameInstance::Get().SetSeletObject(
                        object.get());
                }

                if (opened)
                {
                 

                    if (object->GetObjectINFO().ContainerObject) {
                        
                    }
                    else {
                        auto& components =
                            object->GetComponents();

                        for (auto& component : components)
                        {
                            std::string compName =
                                WStringToString(component.first);

                            OutputDebugStringA(
                                ("[" + compName + "]\n").c_str());
                            ImGui::BulletText(
                                "%s",
                                compName.c_str());
                        }
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }


    IMGUI_ChoiceObject();
    ImGui::End();
}
void MapEditor::IMGUI_OTHER_Render()
{
    ImGui::Begin("Map Editor");
    // =====================================================
    // Performance
    // =====================================================

    if (ImGui::CollapsingHeader(u8"Performance",
        ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("FPS : %.1f",
            ImGui::GetIO().Framerate);

        ImGui::Text("Frame Time : %.3f ms",
            1000.0f / ImGui::GetIO().Framerate);
    }

    ImGui::Separator();
    // =====================================================
    // Prototype Manager
    // =====================================================

    if (ImGui::CollapsingHeader(u8"Prototype Manager",
        ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto names =
            CGameInstance::Get().Get_PrototypeNames(
                CGameInstance::Get().Get_Level());

        for (auto& name : names)
        {
            ImGui::Text("%ls", name.c_str());
        }
    }

    ImGui::Separator();

    // =====================================================
    // Camera Info
    // =====================================================

    if (ImGui::CollapsingHeader(u8"Camera Info",
        ImGuiTreeNodeFlags_DefaultOpen))
    {
        _float4x4 ViewMatrix;
        _float4x4 ProjectionMatrix;

        auto pCamera =
            CGameInstance::Get().Find_Camera(
                ETOUI(CAMERA::MAIN)).lock();

        if (pCamera)
        {
            pCamera->GetShaderMatrix(
                ViewMatrix,
                ProjectionMatrix);

            _matrix view =
                XMLoadFloat4x4(&ViewMatrix);

            _matrix invView =
                XMMatrixInverse(nullptr, view);

            _float4x4 InvView{};

            XMStoreFloat4x4(&InvView, invView);

            ImGui::Text("Position");

            ImGui::Text("X : %.3f", InvView._41);
            ImGui::Text("Y : %.3f", InvView._42);
            ImGui::Text("Z : %.3f", InvView._43);

            ImGui::Separator();

            ImGui::Text("View Matrix");

            ImGui::Text("%.3f %.3f %.3f %.3f",
                ViewMatrix._11,
                ViewMatrix._12,
                ViewMatrix._13,
                ViewMatrix._14);

            ImGui::Text("%.3f %.3f %.3f %.3f",
                ViewMatrix._21,
                ViewMatrix._22,
                ViewMatrix._23,
                ViewMatrix._24);

            ImGui::Text("%.3f %.3f %.3f %.3f",
                ViewMatrix._31,
                ViewMatrix._32,
                ViewMatrix._33,
                ViewMatrix._34);

            ImGui::Text("%.3f %.3f %.3f %.3f",
                ViewMatrix._41,
                ViewMatrix._42,
                ViewMatrix._43,
                ViewMatrix._44);
        }
    }

    ImGui::Separator();

    



    ImGui::End();
}
void MapEditor::IMGUI_SaveLoad_Render()
{
    ImGui::Begin("SaveLoad Editor");

    // =====================================================
    // Save Load
    // =====================================================

    if (ImGui::CollapsingHeader(u8"Save Load",
        ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto& mapNames =
            CGameInstance::Get().GetMapNames();

        static string currentMap;

        ImGui::Text("Load Scene");

        ImGui::PushStyleVar(
            ImGuiStyleVar_FramePadding,
            ImVec2(10.f, 8.f));

        ImGui::PushStyleColor(
            ImGuiCol_Header,
            ImVec4(0.20f, 0.22f, 0.25f, 1.0f));

        ImGui::PushStyleColor(
            ImGuiCol_HeaderHovered,
            ImVec4(0.30f, 0.32f, 0.35f, 1.0f));

        ImGui::PushStyleColor(
            ImGuiCol_HeaderActive,
            ImVec4(0.15f, 0.50f, 0.75f, 1.0f));

        for (auto& name : mapNames)
        {
            bool selected =
                (currentMap == name);

            if (ImGui::Selectable(
                name.c_str(),
                selected,
                ImGuiSelectableFlags_None,
                ImVec2(250.f, 35.f)))
            {
                currentMap = name;

                CGameInstance::Get().Load(
                    name,
                    CGameInstance::Get().Get_Level());
            }

            ImGui::Spacing();
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

        ImGui::Separator();

        ImGui::Text("Save Scene");

        static char saveFileName[128] = "";

        ImGui::InputText(
            "File Name",
            saveFileName,
            IM_ARRAYSIZE(saveFileName));

        if (ImGui::Button("Save"))
        {
            HRESULT result =
                CGameInstance::Get().Save(
                    saveFileName,
                    false);

            if (result == E_FAIL)
            {
                ImGui::OpenPopup("Overwrite?");
            }
        }

        if (ImGui::BeginPopupModal(
            "Overwrite?",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"같은 파일 존재");
            ImGui::Text(u8"덮어쓸까?");

            if (ImGui::Button(u8"예"))
            {
                CGameInstance::Get().Save(
                    saveFileName,
                    true);

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button(u8"아니오"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }


    ImGui::End();
}
void MapEditor::IMGUI_MadeFunction()
{
    ImGui::Begin("Object Browser");

    ImGui::Text("Objects");
    ImGui::Separator();

    const ImVec2 buttonSize(220.f, 50.f);

    // =====================================================
    // Monster
    // =====================================================

    ImGui::PushID("Monster");

    if (ImGui::Button("Monster", buttonSize))
    {
    }

    if (ImGui::BeginDragDropSource())
    {
        string className = "Monster";

        ImGui::SetDragDropPayload(
            "GAME_OBJECT",
            className.c_str(),
            className.size() + 1);

        ImGui::Text("Create Monster");

        ImGui::EndDragDropSource();
    }

    ImGui::PopID();

    ImGui::Spacing();

    // =====================================================
    // Obstacle
    // =====================================================

    ImGui::PushID("Obstacle");

    if (ImGui::Button("Obstacle", buttonSize))
    {
    }

    if (ImGui::BeginDragDropSource())
    {
        string className = "Obstacle";

        ImGui::SetDragDropPayload(
            "GAME_OBJECT",
            className.c_str(),
            className.size() + 1);

        ImGui::Text("Create Obstacle");

        ImGui::EndDragDropSource();
    }

    ImGui::PopID();

    ImGui::End();

}
void MapEditor::IMGUI_ChoiceObject()
{
   
    if (ImGui::BeginPopupModal(
        "Select Monster",
        nullptr))
    {
        static char monsterObjectName[128] = "";
        static char monsterLayerName[128] = "";
        static float speed = 5.f;
        static float rotation = 1.f;

        static int selectedSkeletonIndex = -1;

        auto& SkeletonNames =
            CGameInstance::Get().FindCategories("Skeleton");

        ImGui::Text("Create Monster");
        ImGui::Separator();

        ImGui::InputText(
            "Object Name",
            monsterObjectName,
            IM_ARRAYSIZE(monsterObjectName));

        ImGui::InputText(
            "Layer Name",
            monsterLayerName,
            IM_ARRAYSIZE(monsterLayerName));

        ImGui::DragFloat(
            "Speed",
            &speed,
            0.1f,
            0.f,
            100.f);

        ImGui::DragFloat(
            "Rotation",
            &rotation,
            0.1f,
            0.f,
            100.f);

        ImGui::Separator();

        ImGui::Text("Select Skeleton");

        for (int i = 0; i < SkeletonNames.size(); ++i)
        {
            bool selected =
                selectedSkeletonIndex == i;

            if (ImGui::Selectable(
                SkeletonNames[i].c_str(),
                selected,
                ImGuiSelectableFlags_DontClosePopups))
            {
                selectedSkeletonIndex = i;
            }
        }

        ImGui::Separator();

        if (ImGui::Button(
            "Create",
            ImVec2(120.f, 35.f)))
        {
            if (selectedSkeletonIndex >= 0)
            {
                GameObject::GAMEOBJECT_DESC desc{};

                wstring selectedSkeleton =
                    CGameInstance::Get().StringToWString(
                        SkeletonNames[selectedSkeletonIndex]);

                desc.ObjectType =
                    ETOUI(OBJECTTYPE::OBJECT_MONSTER);

                desc.m_strName =
                    CGameInstance::Get().StringToWString(
                        monsterObjectName);

                desc.m_strPrototypeObjectName =
                    L"Prototype_GameObject_Monster";

                desc.m_strPrototypeBaseName =
                    selectedSkeleton;

                desc.pCameraType =
                    ETOUI(CAMERA::NONE);

                desc.fSpeedPerSec =
                    speed;

                desc.fRotationPerSec =
                    rotation;

                CGameInstance::Get().Add_GameObject_toLayer(
                    CGameInstance::Get().Get_Level(),
                    TEXT("Prototype_GameObject_Monster"),
                    CGameInstance::Get().Get_Level(),
                    CGameInstance::Get().StringToWString(
                        monsterLayerName),
                    &desc);

                selectedSkeletonIndex = -1;

                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button(
            "Cancel",
            ImVec2(120.f, 35.f)))
        {
            selectedSkeletonIndex = -1;

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal(
        "Select Obstacle",
        nullptr))
    {
        static char obstacleObjectName[128] = "";
        static char obstacleLayerName[128] = "";

        static bool useCollider = false;

        static wstring selectedMeshName = L"";
        static wstring selectedMeshPath = L"";
        static string selectedDisplayName = "";

        
        filesystem::path rootPath =
            L"../../Resources/Model/StaticMesh";

        ImGui::Text("Create StaticMesh");
        ImGui::Separator();

        ImGui::InputText(
            "Object Name",
            obstacleObjectName,
            IM_ARRAYSIZE(obstacleObjectName));

        ImGui::InputText(
            "Layer Name",
            obstacleLayerName,
            IM_ARRAYSIZE(obstacleLayerName));

        ImGui::Separator();

        ImGui::Text("Select StaticMesh");
        ImGui::Separator();

        ImGui::BeginChild(
            "StaticMeshTree",
            ImVec2(0.f, 300.f),
            true);

        ShowStaticMeshTree(
            rootPath,
            selectedMeshName,
            selectedMeshPath,
            selectedDisplayName);

        ImGui::EndChild();

        ImGui::Separator();

        if (selectedMeshName.empty())
        {
            ImGui::TextDisabled("Selected : None");
        }
        else
        {
            ImGui::Text("Selected : %s", selectedDisplayName.c_str());
        }

        ImGui::Separator();

        ImGui::Checkbox(
            "Use Collider",
            &useCollider);

        ImGui::Separator();

        bool canCreate =
            !selectedMeshName.empty()
            && strlen(obstacleObjectName) > 0
            && strlen(obstacleLayerName) > 0;

        if (!canCreate)
            ImGui::BeginDisabled();

        if (ImGui::Button(
            "Create",
            ImVec2(120.f, 35.f)))
        {
            GameObject::GAMEOBJECT_DESC desc{};

            desc.ObjectType =
                ETOUI(OBJECTTYPE::OBJECT_STATIC);

            wstring inputName =
                CGameInstance::Get().StringToWString(
                    obstacleObjectName);
            wstring inputLayer =
                CGameInstance::Get().StringToWString(
                    obstacleLayerName);

            if (inputName.empty())
                inputName = selectedMeshName;

            desc.m_strName =
                Make_UniqueObjectName(inputLayer, inputName);

            desc.m_strPrototypeObjectName =
                L"Prototype_GameObject_Obstacle";

            desc.m_strPrototypeBaseName =
                selectedMeshName;

            desc.m_bCollider =
                useCollider;

            desc.pCameraType =
                ETOUI(CAMERA::NONE);

            CGameInstance::Get().Add_GameObject_toLayer(
                CGameInstance::Get().Get_Level(),
                TEXT("Prototype_GameObject_Obstacle"),
                CGameInstance::Get().Get_Level(),
                CGameInstance::Get().StringToWString(obstacleLayerName),
                &desc);
    
            CGameInstance::Get().SetSeletObject(CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), inputLayer, desc.m_strName).get());

            selectedMeshName.clear();
            selectedMeshPath.clear();
            selectedDisplayName.clear();

            ImGui::CloseCurrentPopup();
        }

        if (!canCreate)
            ImGui::EndDisabled();

        ImGui::SameLine();

        if (ImGui::Button(
            "Cancel",
            ImVec2(120.f, 35.f)))
        {
            selectedMeshName.clear();
            selectedMeshPath.clear();
            selectedDisplayName.clear();

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

}
void MapEditor::IMGUI_AddPlayer()
{
    ImGui::Begin("Player");

    if (ImGui::Button(
        "Create Player",
        ImVec2(200.f, 40.f))) {
        CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player"), CGameInstance::Get().Get_Level(), L"PlayerTag");


        GameObject::GAMEOBJECT_DESC descPlayer_Camera{};

        descPlayer_Camera.ObjectType = ETOUI(OBJECTTYPE::OBJECT_CAMERA);
        descPlayer_Camera.m_strName = L"Player_Camera";
        descPlayer_Camera.m_strPrototypeObjectName = L"Prototype_GameObject_PlayerCamera";
        descPlayer_Camera.m_strPrototypeBaseName = L"Prototype_GameObject_PlayerCamera";
        descPlayer_Camera.pCameraType = ETOUI(CAMERA::PLAYER);
        descPlayer_Camera.fSpeedPerSec = 10.f;
        descPlayer_Camera.fRotationPerSec = 0.1f;

        CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_PlayerCamera"), CGameInstance::Get().Get_Level(), L"PlayerTag", &descPlayer_Camera);

        CGameInstance::Get().Add_Camera(ETOUI(CAMERA::PLAYER), dynamic_pointer_cast<Camera>(CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player_Camera")));


    }
      
    ImGui::Separator();

    // =====================================================
    // Camera 변경
    // =====================================================
    static int currentCamera =
        (int)CAMERA::MAIN;

    const char* cameraNames[] =
    {
        "MAIN",
        "SUB",
        "NONE",
        "PLAYER"
    };

    if (ImGui::Combo(
        "Camera Type",
        &currentCamera,
        cameraNames,
        IM_ARRAYSIZE(cameraNames)))
    {
        CGameInstance::Get().Change_Camera(
            (uint32_t)currentCamera);
    }

    ImGui::End();

}
void MapEditor::MousePicking()
{

	if (CGameInstance::Get().Mouse_Down(MOUSEKEYSTATE::DIM_LB) && CGameInstance::Get().Key_Pressing(DIK_LCONTROL))
	{
		
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		float mouseX = (float)pt.x;
		float mouseY = (float)pt.y;

		float width = CGameInstance::Get().Get_ViewportSize().x;
		float height = CGameInstance::Get().Get_ViewportSize().y;
	
		float px = mouseX / (width *0.5f)- 1.0f;
		float py = mouseY / -(height*0.5f) + 1.0f;

		_float4x4 proj, view, Trans;
		CGameInstance::Get().Get_MainCamerwaViewMatrix(view);
		CGameInstance::Get().Get_MainCamerwaProjectionMatrix(proj);
		CGameInstance::Get().GetWorldMatrix(Trans);

		XMMATRIX matProj = XMLoadFloat4x4(&proj);
		XMMATRIX matView = XMLoadFloat4x4(&view);

		XMMATRIX invProj = XMMatrixInverse(nullptr, matProj);
		XMMATRIX invView = XMMatrixInverse(nullptr, matView);

		// Ray 생성
		XMVECTOR rayClip = XMVectorSet(px, py, 0.f, 1.0f);

		// Near
		XMVECTOR nearPoint = XMVectorSet(px, py, 0.0f, 1.0f);
		nearPoint = XMVector3TransformCoord(nearPoint, invProj);
		nearPoint = XMVector3TransformCoord(nearPoint, invView);

		// Far
		XMVECTOR farPoint = XMVectorSet(px, py, 1.0f, 1.0f);
		farPoint = XMVector3TransformCoord(farPoint, invProj);
		farPoint = XMVector3TransformCoord(farPoint, invView);

		// Ray
		XMVECTOR rayOrigin = nearPoint;
		XMVECTOR rayDir = XMVector3Normalize(farPoint - nearPoint);

		XMVECTOR rayView = XMVector3TransformCoord(rayClip, invProj);
		rayView = XMVectorSet(rayView.m128_f32[0], rayView.m128_f32[1], 0.0f, 0.0f);


		float minDist = FLT_MAX;
        CGameInstance::Get().MousePicking(rayOrigin, rayDir, CGameInstance::Get().Get_Level());


	}
	
}

void MapEditor::FindStaticMeshFiles(
    vector<string>& outDisplayNames,
    vector<wstring>& outMeshNames)
{
    outDisplayNames.clear();
    outMeshNames.clear();
    namespace fs = std::filesystem;
    fs::path rootPath = L"../../Resources/Model/StaticMesh";

    if (!fs::exists(rootPath))
        return;

    for (const auto& entry : fs::recursive_directory_iterator(rootPath))
    {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension() != L".bin")
            continue;

        bool isNonAnim = false;

        for (const auto& part : entry.path())
        {
            if (part == L"NonAnim")
            {
                isNonAnim = true;
                break;
            }
        }

        if (!isNonAnim)
            continue;

        fs::path relativePath = fs::relative(entry.path(), rootPath);

        wstring categoryName = L"";
        if (!relativePath.empty())
            categoryName = (*relativePath.begin()).wstring();

        wstring meshName = entry.path().stem().wstring();

        string displayName =
            CGameInstance::Get().WStringToString(categoryName)
            + " / "
            + CGameInstance::Get().WStringToString(meshName);

        outDisplayNames.push_back(displayName);
        outMeshNames.push_back(meshName);
    }
}

void  MapEditor::ShowStaticMeshTree(
    const std::filesystem::path& rootPath,
    wstring& selectedMeshName,
    wstring& selectedMeshPath,
    string& selectedDisplayName)
{
    namespace fs = std::filesystem;
    if (!fs::exists(rootPath))
    {
        ImGui::TextDisabled("StaticMesh folder not found.");
        return;
    }

    vector<fs::path> categoryFolders;

    for (const auto& categoryEntry : fs::directory_iterator(rootPath))
    {
        if (categoryEntry.is_directory())
            categoryFolders.push_back(categoryEntry.path());
    }

    sort(
        categoryFolders.begin(),
        categoryFolders.end(),
        [](const fs::path& a, const fs::path& b)
        {
            return a.filename().wstring() < b.filename().wstring();
        });

    for (const auto& categoryPath : categoryFolders)
    {
        wstring categoryNameW = categoryPath.filename().wstring();
        string categoryName =
            CGameInstance::Get().WStringToString(categoryNameW);

        if (ImGui::TreeNodeEx(
            categoryName.c_str(),
            ImGuiTreeNodeFlags_DefaultOpen))
        {
            vector<fs::path> typeFolders;

            for (const auto& typeEntry : fs::directory_iterator(categoryPath))
            {
                if (typeEntry.is_directory())
                    typeFolders.push_back(typeEntry.path());
            }

            sort(
                typeFolders.begin(),
                typeFolders.end(),
                [](const fs::path& a, const fs::path& b)
                {
                    return a.filename().wstring() < b.filename().wstring();
                });

            for (const auto& typePath : typeFolders)
            {
                wstring typeNameW = typePath.filename().wstring();
                string typeName =
                    CGameInstance::Get().WStringToString(typeNameW);

                if (ImGui::TreeNodeEx(
                    typeName.c_str(),
                    ImGuiTreeNodeFlags_DefaultOpen))
                {
                    vector<fs::path> modelFolders;

                    for (const auto& modelEntry : fs::directory_iterator(typePath))
                    {
                        if (modelEntry.is_directory())
                            modelFolders.push_back(modelEntry.path());
                    }

                    sort(
                        modelFolders.begin(),
                        modelFolders.end(),
                        [](const fs::path& a, const fs::path& b)
                        {
                            return a.filename().wstring() < b.filename().wstring();
                        });

                    if (modelFolders.empty())
                    {
                        ImGui::TextDisabled("No model folders");
                    }

                    for (const auto& modelPath : modelFolders)
                    {
                        wstring modelNameW = modelPath.filename().wstring();
                        string modelName =
                            CGameInstance::Get().WStringToString(modelNameW);

                        fs::path binPath = modelPath / (modelNameW + L".bin");

                        bool hasBin = fs::exists(binPath);

                        string itemName = modelName;

                        if (!hasBin)
                            itemName += "  [bin missing]";

                        string displayName =
                            categoryName
                            + " / "
                            + typeName
                            + " / "
                            + modelName;

                        bool selected =
                            selectedMeshPath == binPath.wstring();

                        if (!hasBin)
                            ImGui::BeginDisabled();

                        if (ImGui::Selectable(
                            itemName.c_str(),
                            selected,
                            ImGuiSelectableFlags_DontClosePopups))
                        {
                            selectedMeshName = modelNameW;
                            selectedMeshPath = binPath.wstring();
                            selectedDisplayName = displayName;
                        }

                        if (!hasBin)
                            ImGui::EndDisabled();

                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();

                            ImGui::Text("Model : %s", modelName.c_str());
                            ImGui::Text("Type  : %s", typeName.c_str());
                            ImGui::Text("Group : %s", categoryName.c_str());

                            string pathText =
                                CGameInstance::Get().WStringToString(
                                    binPath.wstring());

                            ImGui::Separator();
                            ImGui::TextWrapped("%s", pathText.c_str());

                            ImGui::EndTooltip();
                        }
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }
}

wstring MapEditor::Make_UniqueObjectName(const wstring& LayerName, const wstring& baseName)
{
    wstring finalName = baseName;

    if (finalName.empty())
        finalName = L"Object";

    if (!CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), LayerName,finalName))
        return finalName;

    for (uint32_t i = 1; i < 10000; ++i)
    {
        wchar_t szNumber[32] = {};
        swprintf_s(szNumber, L"_%03d", i);

        wstring newName = finalName + szNumber;

        if (!CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), LayerName, newName))
            return newName;
    }

    return finalName;
}