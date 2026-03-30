#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
public:
	virtual ~CGameInstance();

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& Engine_Desc, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>&pOutDeviceContext);
	void	Update_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear_Resource(uint32_t iClearLevelIndex);
public:
#pragma region TIME_MANAGER
	_float			Get_TimeDelta(const _wstring& strTimerTag);
	void			Set_TimeDelta(const _wstring& strTimerTag);
	HRESULT			Add_Timer(const _wstring& strTimerTag);
#pragma endregion

#pragma region GRAPHIC_DEVICE
	HRESULT			Clear_BackBuffer_View(const _float4* pClearColor);
	HRESULT			Clear_DepthStencil_View();
	HRESULT			Present();
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT			Change_Level(uint32_t iNewLevelIndex, unique_ptr<class CLevel> pNewLevel);
#pragma endregion


#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, unique_ptr<class Prototype> pPrototype);
	shared_ptr<Prototype> Clone_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion


#pragma region GAMEOBJECT_MANAGER
	HRESULT Add_GameObject_toLayer(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag,
		uint32_t iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
#pragma endregion

#pragma region IMGUI_MANAGER
	HRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#pragma endregion

private:
	unique_ptr<class Graphic_Device>				m_pGraphic_Device = { nullptr };
	unique_ptr<class ImGUI_Manager>					m_pImGUI_Manager = { nullptr };
	unique_ptr<class CTimer_Manager>				m_pTimer_Manager = { nullptr };
	unique_ptr<class Level_Manager>					m_pLevel_Manager = { nullptr};
	unique_ptr<class Prototype_Manager>				m_pPrototype_Manager = { nullptr };
	unique_ptr<class Object_Manager>				m_pObject_Manager = { nullptr };


public:
	void			Release_Engine();

};

NS_END