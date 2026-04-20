
#include "MapEditor.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Layer.h"
MapEditor::MapEditor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

MapEditor::~MapEditor()
{
}

HRESULT MapEditor::Initialize()
{
	Ready_Layer_MapEditor(L"MapEditorLayer");

	CGameInstance::Get().Change_Camera(ETOUI(CAMERA::MAIN));

	return S_OK;
}

void MapEditor::Update(_float fTimeDelta)
{


}

HRESULT MapEditor::Ready_Layer_MapEditor(const _wstring& strLayerTag)
{

	GameObject::GAMEOBJECT_DESC descTestModel0{};
	descTestModel0.m_strName =L"TestModel0";
	descTestModel0.pCameraType = ETOUI(CAMERA::NONE);
	descTestModel0.fSpeedPerSec = 5.f;
	descTestModel0.fRotationPerSec = XMConvertToRadians(90.0f);

 	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_GameObject_TestModel"),
		ETOUI(LEVEL::MAPEDITOR), strLayerTag, &descTestModel0) ))
		return E_FAIL;



	GameObject::GAMEOBJECT_DESC descMain_Camera{};
	descMain_Camera.m_strName = L"Main_Camera";
	descMain_Camera.pCameraType = ETOUI(CAMERA::MAIN);
	descMain_Camera.fSpeedPerSec = 1.f;
	descMain_Camera.fRotationPerSec = XMConvertToRadians(10.f);


	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_GameObject_PerspectiveCamera"),
		ETOUI(LEVEL::MAPEDITOR), strLayerTag, &descMain_Camera)))
		return E_FAIL;
	
	if(FAILED(CGameInstance::Get().Add_Camera(ETOUI(CAMERA::MAIN),
		dynamic_pointer_cast<Camera>(CGameInstance::Get().Find_Object(ETOUI(LEVEL::MAPEDITOR), L"MapEditorLayer", L"Main_Camera")))))
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

#endif

}

void MapEditor::IMGUI_Level_Render()
{// --------------------------------Levels -----------------------------------------------------
	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	float leftPanelWidth = 260.f;

	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y));
	ImGui::SetNextWindowSize(
		ImVec2(leftPanelWidth, viewport->WorkSize.y),
		ImGuiCond_Once  
	);
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse;

	ImGui::Begin("Level", nullptr, flags);



	for (auto& layer : CGameInstance::Get().Find_Layer_Lists(ETOUI(LEVEL::MAPEDITOR)))
	{
		std::string layerLabel = WStringToString(layer.first);

		if (ImGui::TreeNode(layerLabel.c_str()))
		{
			auto& gameObjects = layer.second->Get_GameObjects();

			for (auto& object : gameObjects)
			{
				std::string objectName = WStringToString(object->GetObjectName());

				ImGuiTreeNodeFlags flags =
					ImGuiTreeNodeFlags_OpenOnArrow |
					ImGuiTreeNodeFlags_SpanAvailWidth |
					(CGameInstance::Get().GetSelectObject() == object.get()
						? ImGuiTreeNodeFlags_Selected
						: 0);

				bool opened = ImGui::TreeNodeEx(
					(objectName + "##" + std::to_string((uintptr_t)object.get())).c_str(),
					flags
				);

	
				if (ImGui::IsItemClicked())
				{
					CGameInstance::Get().SetSeletObject(object.get());
				}

				if (opened)
				{
					auto& components = object->GetComponents();

					for (auto& component : components)
					{
						std::string compName = WStringToString(component.first);

					
						ImGui::BulletText("%s", compName.c_str());
					}

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
}

void MapEditor::IMGUI_OTHER_Render()
{
	ImGui::Begin(u8"Prototype Manager", nullptr, ImGuiWindowFlags_NoTitleBar);
	ImGui::Text(u8"초기 프로토타입");

	auto names = CGameInstance::Get().Get_PrototypeNames(ETOUI(LEVEL::MAPEDITOR));
	for (auto& name : names)
	{
		ImGui::Text(u8"%ls", name.c_str());
	}
	ImGui::End();

	// ---------------------------------Camera Position-------------------------------------------
	_float4x4 ViewMatrix;
	_float4x4 ProjectionMatrix;
	auto pCamera = CGameInstance::Get().Find_Camera(ETOUI(CAMERA::MAIN)).lock();
	if (pCamera)
	{
		pCamera->GetShaderMatrix(ViewMatrix, ProjectionMatrix);

		_matrix view = XMLoadFloat4x4(&ViewMatrix);
		_matrix invView = XMMatrixInverse(nullptr, view);

		_float4x4 InvView{};
		XMStoreFloat4x4(&InvView, invView);

		ImGui::Begin("Camera Info");

		ImGui::Text("Position");
		ImGui::Text("X : %.3f", InvView._41);
		ImGui::Text("Y : %.3f", InvView._42);
		ImGui::Text("Z : %.3f", InvView._43);

		ImGui::Separator();

		ImGui::Text("View Matrix");
		ImGui::Text("%.3f %.3f %.3f %.3f", ViewMatrix._11, ViewMatrix._12, ViewMatrix._13, ViewMatrix._14);
		ImGui::Text("%.3f %.3f %.3f %.3f", ViewMatrix._21, ViewMatrix._22, ViewMatrix._23, ViewMatrix._24);
		ImGui::Text("%.3f %.3f %.3f %.3f", ViewMatrix._31, ViewMatrix._32, ViewMatrix._33, ViewMatrix._34);
		ImGui::Text("%.3f %.3f %.3f %.3f", ViewMatrix._41, ViewMatrix._42, ViewMatrix._43, ViewMatrix._44);

		ImGui::Separator();

		ImGui::Text("Projection Matrix");
		ImGui::Text("%.3f %.3f %.3f %.3f", ProjectionMatrix._11, ProjectionMatrix._12, ProjectionMatrix._13, ProjectionMatrix._14);
		ImGui::Text("%.3f %.3f %.3f %.3f", ProjectionMatrix._21, ProjectionMatrix._22, ProjectionMatrix._23, ProjectionMatrix._24);
		ImGui::Text("%.3f %.3f %.3f %.3f", ProjectionMatrix._31, ProjectionMatrix._32, ProjectionMatrix._33, ProjectionMatrix._34);
		ImGui::Text("%.3f %.3f %.3f %.3f", ProjectionMatrix._41, ProjectionMatrix._42, ProjectionMatrix._43, ProjectionMatrix._44);

		ImGui::End();
	}
	// ---------------------------------frame--------------------------------------------
	ImGui::Begin("Performance");


	ImGui::Text("FPS        : %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("Frame Time : %.3f ms", 1000.0f / ImGui::GetIO().Framerate);

	ImGui::End();
}

