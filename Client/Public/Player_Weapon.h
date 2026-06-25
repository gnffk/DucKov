#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class BaseCollider;
class Shader;
class Model;
NS_END


NS_BEGIN(Client)

class Player_Weapon final : public PartObject
{
public:
	typedef struct tagWeaponDesc : public PartObject::PARTOBJECT_DESC
	{
		const uint32_t* pParentState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
	}WEAPON_DESC;

private:
	typedef struct tagWeaponSetting
	{
		wstring strModelPrototypeTag = L"";

		_float3 vLocalPos = { 0.f, 0.f, 0.f };
		_float3 vLocalRot = { 0.f, 0.f, 0.f };
		_float3 vLocalScale = { 1.f, 1.f, 1.f };

		_float3 vMuzzleLocalPos = { 0.f, 0.f, 1.f };

		float fFireCoolTime = 0.2f;
		float fBulletSpeed = 60.f;

		float fMouseRecoilPower = 70.f;
		float fMouseRecoilPowerRandom = 10.f;
		float fMouseRecoilSideRandom = 12.f;
		float fMouseRecoilMaxOffset = 220.f;

		float fMouseRecoilKickSpeed = 550.f;
		float fMouseRecoilRecoverSpeed = 180.f;


		// 추가
		std::wstring strFireSoundKey = L"";
		_float fFireSoundVolume = 0.7f;

	} WEAPON_SETTING;

private:
	Player_Weapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Player_Weapon(const Player_Weapon& Prototype);
public:
	virtual ~Player_Weapon();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	map<string, shared_ptr<Model>> m_pWeaponModelMap;
	shared_ptr<Shader>				m_pShaderCom = { nullptr };
	map<string, WEAPON_SETTING>    m_WeaponSettingMap;

	string m_strCurrentWeaponType = "Default";
	WEAPON_SETTING m_CurrentWeaponSetting{};
private:
	const uint32_t* m_pParentState = { nullptr };
	const _float4x4* m_pSocketMatrix = { nullptr };

private:
	_float3 m_vMuzzleWorldPos = {};
	_float3 m_vBulletDir = { 0.f, 0.f, 1.f };

public:
	void Set_WeaponType(const string& strWeaponType);
	const string& Get_WeaponType() const;

private:
	void Ready_WeaponSettings();

	// 마우스 반동
private:
	void Apply_MouseRecoil(_vector vBulletDir);
	void Update_MouseRecoil(_float fTimeDelta, _bool bIsShooting);
	_bool Make_ScreenDirFromWorldDir(const _float3& vWorldPos, _vector vWorldDir, _float2& vOutScreenDir);

private:
	_bool m_bMouseRecoilActive = false;

	POINT m_ptMouseRecoilBase{};

	_float2 m_vMouseRecoilTargetOffset = { 0.f, 0.f };   // 목표 반동 위치
	_float2 m_vMouseRecoilAppliedOffset = { 0.f, 0.f };  // 실제 현재 적용된 반동
	_float2 m_vPrevAppliedMouseRecoil = { 0.f, 0.f };

	_float m_fMouseRecoilPower = 70.f;
	_float m_fMouseRecoilPowerRandom = 10.f;
	_float m_fMouseRecoilSideRandom = 12.f;
	_float m_fMouseRecoilMaxOffset = 220.f;

	_float m_fMouseRecoilKickSpeed = 550.f;      // 밀리는 속도
	_float m_fMouseRecoilRecoverSpeed = 180.f;
public:
	void Fire_Bullet();

private:
	_float m_fFireCoolTime = 0.2f;
	_float m_fFireTimer = 0.f;

private:
	HRESULT Ready_Components();
#ifdef _DEBUG
private:
	void GUI_PlayerWeapon();
#endif
public:
	static unique_ptr<Player_Weapon> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END