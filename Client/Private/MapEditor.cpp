
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

    if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_GameObject_Terrain"),
        ETOUI(LEVEL::MAPEDITOR), strLayerTag, &descTerrian)))
        return E_FAIL;


    GameObject::GAMEOBJECT_DESC descSKY{};
    descSKY.ObjectType = ETOUI(OBJECTTYPE::OBJECT_SKYBOX);
    descSKY.m_strName = L"SkyBox";
    descSKY.m_strPrototypeObjectName = L"Prototype_GameObject_Sky";
    descSKY.m_strPrototypeBaseName = L"SkyBox";
    descSKY.pCameraType = ETOUI(CAMERA::NONE);
    descSKY.fSpeedPerSec = 5.f;
    descSKY.fRotationPerSec = 1.f;

    if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_GameObject_Sky"),
        ETOUI(LEVEL::MAPEDITOR), strLayerTag, &descSKY)))
        return E_FAIL;


 //  
	//GameObject::GAMEOBJECT_DESC descTestModel0{};
 //   descTestModel0.ObjectType = ETOUI(OBJECTTYPE::OBJECT_MONSTER);
	//descTestModel0.m_strName =L"TestMonster0";
 //   descTestModel0.m_strPrototypeObjectName = L"Prototype_GameObject_Monster";
	//descTestModel0.m_strPrototypeBaseName =L"SK_Monster_Palicus";
	//descTestModel0.pCameraType = ETOUI(CAMERA::NONE);
	//descTestModel0.fSpeedPerSec = 5.f;
	//descTestModel0.fRotationPerSec = 1.f;

 //   for (int i = 0; i < 100; ++i) {
 //       if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_GameObject_Monster"),
 //           ETOUI(LEVEL::MAPEDITOR), strLayerTag, &descTestModel0)))
 //           return E_FAIL;
 //   }




	GameObject::GAMEOBJECT_DESC descMain_Camera{};

    descMain_Camera.ObjectType = ETOUI(OBJECTTYPE::OBJECT_CAMERA);
	descMain_Camera.m_strName = L"Main_Camera";
    descMain_Camera.m_strPrototypeObjectName = L"Prototype_GameObject_PerspectiveCamera";
	descMain_Camera.m_strPrototypeBaseName = L"Prototype_GameObject_PerspectiveCamera";
	descMain_Camera.pCameraType = ETOUI(CAMERA::MAIN);
	descMain_Camera.fSpeedPerSec = 1.f;
	descMain_Camera.fRotationPerSec = 0.1f;


	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_GameObject_PerspectiveCamera"),
		ETOUI(LEVEL::MAPEDITOR), strLayerTag, &descMain_Camera)))
		return E_FAIL;
	
	if(FAILED(CGameInstance::Get().Add_Camera(ETOUI(CAMERA::MAIN),
		dynamic_pointer_cast<Camera>(CGameInstance::Get().Find_Object(ETOUI(LEVEL::MAPEDITOR), strLayerTag, L"Main_Camera")))))
		return E_FAIL;

	return S_OK;
}


HRESULT MapEditor::Render()
{
	IMGUI_Render();


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
            viewport->WorkPos.y));

    ImGui::SetNextWindowSize(
        ImVec2(
            leftPanelWidth,
            viewport->WorkSize.y),
        ImGuiCond_Once);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Level", nullptr, flags);

    // =====================================================
    // Layer Loop
    // =====================================================

    for (auto& layer :
        CGameInstance::Get().Find_Layer_Lists(
            ETOUI(LEVEL::MAPEDITOR)))
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
                ETOUI(LEVEL::MAPEDITOR));

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
                    ETOUI(LEVEL::MAPEDITOR));
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

    ImGui::Separator();

    // =====================================================
    // Monster 파일
    // =====================================================

    if (ImGui::Selectable(
        "Monster",
        false,
        ImGuiSelectableFlags_None,
        ImVec2(200.f, 40.f)))
    {
    }

    // =====================================================
    // Drag 시작
    // =====================================================

    if (ImGui::BeginDragDropSource())
    {
        string className = "Monster";

        ImGui::SetDragDropPayload(
            "GAME_OBJECT",
            className.c_str(),
            className.size() + 1);

        ImGui::Text("Monster");

        ImGui::EndDragDropSource();
    }

    ImGui::End();


}
void MapEditor::IMGUI_ChoiceObject()
{
   
    if (ImGui::BeginPopupModal(
        "Select Monster",
        nullptr))
    {
        static char objectName[128] = "";
        static char layerName[128] = "";
        static float speed = 5.f;
        static float rotation = 1.f;

        static int selectedSkeletonIndex = -1;

        auto& SkeletonNames =
            CGameInstance::Get().FindCategories("Skeleton");

        ImGui::Text("Create Monster");
        ImGui::Separator();

        // =====================================================
        // 이름 입력
        // =====================================================

        ImGui::InputText(
            "Object Name",
            objectName,
            IM_ARRAYSIZE(objectName));


        ImGui::InputText(
            "Layer Name",
            layerName,
            IM_ARRAYSIZE(layerName));
        // =====================================================
        // 이동 속도
        // =====================================================

        ImGui::DragFloat(
            "Speed",
            &speed,
            0.1f,
            0.f,
            100.f);

        // =====================================================
        // 회전 속도
        // =====================================================

        ImGui::DragFloat(
            "Rotation",
            &rotation,
            0.1f,
            0.f,
            100.f);

        ImGui::Separator();

        ImGui::Text("Select Skeleton");

        // =====================================================
        // Skeleton 선택
        // =====================================================

        for (int i = 0; i < SkeletonNames.size(); ++i)
        {
            bool selected =
                selectedSkeletonIndex == i;

            if (ImGui::Selectable(
                SkeletonNames[i].c_str(),
                selected, ImGuiSelectableFlags_DontClosePopups))
            {
                selectedSkeletonIndex = i;
            }
        }

        ImGui::Separator();

        // =====================================================
        // 생성 버튼
        // =====================================================

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
                        objectName);

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
                    ETOUI(LEVEL::MAPEDITOR),
                    TEXT("Prototype_GameObject_Monster"),
                    ETOUI(LEVEL::MAPEDITOR),
                    CGameInstance::Get().StringToWString(
                        layerName),
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



}
void MapEditor::MousePicking()
{

	if (CGameInstance::Get().Mouse_Down(MOUSEKEYSTATE::DIM_LB))
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
        CGameInstance::Get().MousePicking(rayOrigin, rayDir, ETOUI(LEVEL::MAPEDITOR));


	}
	
}

