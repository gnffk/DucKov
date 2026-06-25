#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"
#include "UIObject.h"
NS_BEGIN(Client)

class Player final : public ContainerObject
{


public:
	typedef struct tagPlayerDesc : public ContainerObject::CONTAINEROBJECT_DESC
	{

	}PLAYER_DESC;

private:
	Player(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Player(const Player& Prototype);
public:
	virtual ~Player();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;
public:
	void		MouseLook(_float fTimeDelta);
	void		KeyBoardLook(_float fTimeDelta);
	HRESULT		Roll(_float fTimeDelta);
	void		Shift(_float fTimeDelta);
	_vector		Get_RollDir() const
	{
		return m_vRollDir;
	}
	shared_ptr<class Navigation> GetNavigation() { return m_pNavigationCom; }
	map<string, shared_ptr<class GameObject>>& GetUIs() { return m_pUI; }
	void Update_HP_UI();
private:
	uint32_t			m_iState = {};
	

	shared_ptr<class FSM> m_pPlayerFSM = { nullptr };
	shared_ptr<class Navigation> m_pNavigationCom = { nullptr };
	map<string, shared_ptr<class GameObject>> m_pUI ;
	_bool				m_bShift = { false };
	_bool				m_isRolling = { false };
	_float				m_fRollTimer = {};
	_float				m_fRollDuration = { 0.45f };
	_vector				m_vRollDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	float			m_fSpeedFloat{ 0.f };
private:
	_bool m_bPrevHitInteractBox = false;

	_float		m_fMaxHP = 1000.f;
	_float		m_fHP = 1000.f;
	_float		m_fAttackPower = 10.f;


public:
	_bool	m_bNext = false;
private:
#ifdef _DEBUG
	void			IMGUI_DEBUGRENDER();
#endif

public:
	void InvenSet(_bool _bInvenSet) { m_bInventoryOpen = _bInvenSet; }
	_bool GetInven() { return m_bInventoryOpen; }
private:
	_bool m_bInventoryOpen = false;
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_UI();
private:
	void Spawn_BloodEffect(const _float3& vSpawnPos);
public:
	void Take_Damage(_float fDamage, const _float3& vHitPos);


public:
	void Equip_Item(Engine::UIObject::SLOT_KIND eSlotKind, const Engine::UIObject::INV_ITEM& Item);
	void Unequip_Item(Engine::UIObject::SLOT_KIND eSlotKind, const Engine::UIObject::INV_ITEM& Item);

public:
	void Set_WeaponSlot(int iSlotNumber, const Engine::UIObject::INV_ITEM& Item);
	void Clear_WeaponSlot(int iSlotNumber);
	void Switch_WeaponSlot(int iSlotNumber);

public:
	HRESULT Change_Navigation(const wstring& strNavigationPrototypeTag, uint32_t iStartCellIndex = 1);
	HRESULT Change_Navigation_AndWarp(const wstring& strNavigationPrototypeTag, const _float3& vWarpPos, uint32_t iStartCellIndex);
	HRESULT Change_Navigation_AndHome(const wstring& strNavigationPrototypeTag, const _float3& vWarpPos, uint32_t iStartCellIndex);
private:
	wstring m_strCurrentNavigationTag = L"Prototype_Component_Navigation";
private:
	string m_strWeaponSlotKey[2] = { "Default", "Default" };
	int m_iCurrentWeaponSlot = -1;

public:
	PLAYER_SAVE_DATA Make_SaveData() const;
	void Apply_SaveData(const PLAYER_SAVE_DATA& Data);

private:
	_bool Collider_Obstacle(_float fTimeDelta);
	_bool Collider_Box(_float fTimeDelta);
	_bool Collider_HitAttack(_float fTimeDelta);
public:
	static unique_ptr<Player> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END