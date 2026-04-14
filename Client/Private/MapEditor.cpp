
#include "MapEditor.h"
#include "GameInstance.h"

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
	return S_OK;
}

void MapEditor::Update(_float fTimeDelta)
{


}

HRESULT MapEditor::Ready_Layer_MapEditor(const _wstring& strLayerTag)
{
	
	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_GameObject_TestModel"),
		ETOUI(LEVEL::MAPEDITOR), strLayerTag)))
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
	ImGui::Begin(u8"¸Ê ¿¡µðÅÍ", nullptr, ImGuiWindowFlags_NoTitleBar);
	ImGui::Text(u8"¸Ê ¿¡µðÅÍ");

	
	ImGui::End();
#endif

}