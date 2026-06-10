#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

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
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();


private:
	_bool Collider_Obstacle(_float fTimeDelta);
public:
	static unique_ptr<Player> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END