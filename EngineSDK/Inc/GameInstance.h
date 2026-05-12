#pragma once

#include "Prototype_Manager.h"


NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
public:
	virtual ~CGameInstance();

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& Engine_Desc, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext);
	void	Update_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear_Resource(uint32_t iClearLevelIndex);
	void Clear_Resource_SameLevel(uint32_t iClearLevelIndex);
	_float2 Get_ViewportSize() const {
		return m_vViewportSize;
	}
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
	vector<wstring> Get_PrototypeNames(uint32_t levelIndex);
#pragma endregion


#pragma region GAMEOBJECT_MANAGER
	HRESULT Add_GameObject_toLayer(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag,
		uint32_t iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);

	shared_ptr<class GameObject> Find_Object(uint32_t iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strObjectTag);

	map<const _wstring, unique_ptr<class Layer>>& Find_Layer_Lists(uint32_t iLayerLevelIndex);

#pragma endregion

#pragma region IMGUI_MANAGER
	bool WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Render_IMGUI();
	HRESULT SetSeletObject(GameObject* select);
	class GameObject* GetSelectObject();
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<GameObject> pRenderObject);
#pragma endregion

#pragma region Camera_Manager
	HRESULT Change_Camera(uint32_t iCameraType);
	HRESULT Add_Camera(uint32_t iCameraType, shared_ptr<class Camera> pCamera);
	HRESULT Get_MainCameraMatrix(_float4x4& ViewMatrix, _float4x4& ProjectionMatrix);
	HRESULT Get_MainCamerwaViewMatrix(_float4x4& ViewMatrix);
	HRESULT Get_MainCamerwaProjectionMatrix(_float4x4& ProjectionMatrix);
	HRESULT Get_MainCameraWorldMatrix(_float4x4& WorldMatrix);
	void GetWorldMatrix(_float4x4& WorldMatrix);
	weak_ptr<Camera> Find_Camera(uint32_t iCameraType);
#pragma endregion

#pragma region Collider_Manager
	HRESULT Add_Collider(wstring GroupTag, class BaseCollider* pCollider);
	std::vector<BaseCollider*>* GetColliderGroups(wstring GroupTag);
	std::unordered_map<wstring, std::vector<BaseCollider*>>& GetAllCollders();
#pragma endregion

#pragma region KEY_MANAGER
	HRESULT Ready_Key(HINSTANCE hInst, HWND hWnd);

	_uchar	Get_DIKeyState(_uchar byKeyID);

	_uchar	Get_DIMouseState(MOUSEKEYSTATE eMouse);


	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

	bool Key_Pressing(_uchar byKeyID);
	bool Key_Up(_uchar byKeyID);
	bool Key_Down(_uchar byKeyID);

	bool Mouse_Pressing(MOUSEKEYSTATE eMouseState);
	bool Mouse_Up(MOUSEKEYSTATE eMouseState);
	bool Mouse_Down(MOUSEKEYSTATE eMouseState);
#pragma endregion



#pragma region Map_Manager
	HRESULT Save(string _mapDataName, bool _overwrite);
	HRESULT Load(string _mapDataName, uint32_t Levelindex);
	vector<string>& GetMapNames();
#pragma endregion
private:
	_float2											m_vViewportSize = {};
private:
	unique_ptr<class Graphic_Device>				m_pGraphic_Device = { nullptr };
	unique_ptr<class ImGUI_Manager>					m_pImGUI_Manager = { nullptr };
	unique_ptr<class CTimer_Manager>				m_pTimer_Manager = { nullptr };
	unique_ptr<class Level_Manager>					m_pLevel_Manager = { nullptr};
	unique_ptr<class Prototype_Manager>				m_pPrototype_Manager = { nullptr };
	unique_ptr<class Object_Manager>				m_pObject_Manager = { nullptr };
	unique_ptr<class Renderer>						m_pRenderer = { nullptr };
	unique_ptr<class Camera_Manager>				m_pCamera_Manager = { nullptr };
	unique_ptr<class Key_Manager>					m_pKey_Manager = { nullptr };
	unique_ptr<class Collider_Manager>				m_pCollider_Manager = { nullptr };
	unique_ptr<class Map_Manager>					m_pMap_Manager = { nullptr };


public:
	void			Release_Engine();

};

NS_END