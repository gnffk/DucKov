
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
	Ready_Layer_MapEditor(L"MapEditorLayer");

	CGameInstance::Get().Change_Camera(ETOUI(CAMERA::MAIN));

	return S_OK;
}

void MapEditor::Update(_float fTimeDelta)
{

	MousePicking();
}

HRESULT MapEditor::Ready_Layer_MapEditor(const _wstring& strLayerTag)
{

	GameObject::GAMEOBJECT_DESC descTestModel0{};
	descTestModel0.m_strName =L"TestModel0";
	descTestModel0.pCameraType = ETOUI(CAMERA::NONE);
	descTestModel0.fSpeedPerSec = 5.f;
	descTestModel0.fRotationPerSec = 1.f;

 	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_GameObject_TestModel"),
		ETOUI(LEVEL::MAPEDITOR), strLayerTag, &descTestModel0) ))
		return E_FAIL;



	GameObject::GAMEOBJECT_DESC descMain_Camera{};
	descMain_Camera.m_strName = L"Main_Camera";
	descMain_Camera.pCameraType = ETOUI(CAMERA::MAIN);
	descMain_Camera.fSpeedPerSec = 1.f;
	descMain_Camera.fRotationPerSec = 0.1f;


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

	ImGui::Begin("Level", nullptr);



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

		for (auto& CollderGruops : CGameInstance::Get().GetAllCollders())
		{
			auto& vec = CollderGruops.second;

			for (auto collider : vec)
			{
				float dist;
				if (collider->Intersect(rayOrigin, rayDir, dist))
				{
					if (dist < minDist)
					{
						minDist = dist;
						if (CGameInstance::Get().GetSelectObject() != collider->GetOwner()) {

							CGameInstance::Get().SetSeletObject(collider->GetOwner());
						}
						else if (CGameInstance::Get().GetSelectObject() == collider->GetOwner()) {
						

						}
					
						break;
					}
					

				}
				else {
					CGameInstance::Get().SetSeletObject(nullptr);
					break;
				}
			}
		}
	}
	
}

