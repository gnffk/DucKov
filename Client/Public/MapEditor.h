#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

NS_BEGIN(Client)

class MapEditor final : public CLevel
{
public:
	
private:
	MapEditor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~MapEditor();

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	HRESULT Ready_Layer_MapEditor(const _wstring& strLayerTag);
	virtual HRESULT Render() override;
	void	IMGUI_Render();
	void	IMGUI_Level_Render();
	void	IMGUI_OTHER_Render();
	void	IMGUI_SaveLoad_Render();

	void	IMGUI_MadeFunction();
	void	IMGUI_ChoiceObject();
	void	IMGUI_AddPlayer();
	void	IMGUI_TreeBrush_Render();
public:
	void	MousePicking();

	void	FindStaticMeshFiles(vector<string>& outDisplayNames, vector<wstring>& outMeshNames);

	void	ShowStaticMeshTree(const std::filesystem::path& rootPath, wstring& selectedMeshName, wstring& selectedMeshPath, string& selectedDisplayName);

	wstring Make_UniqueObjectName(const wstring& LayerName, const wstring& baseName);

private:
	_bool b_Debug = false;
private:
	Shadow::SHADOW_LIGHT_DESC m_ShadowLightDesc{};

	_float3 m_vShadowEye = { -20.f, 100.f, -5.5f };
	_float3 m_vShadowAt = { 20.f,   5.f,  5.f };

	float m_fShadowFovyDeg = 60.f;
	float m_fShadowNear = 0.1f;
	float m_fShadowFar = 1000.f;

private:
	void IMGUI_ShadowLight_Render();
	void Apply_ShadowLight();
private:
	unique_ptr<class NavMeshEditor> m_pNavMeshEditor = nullptr;

public:
	static unique_ptr<MapEditor> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END


