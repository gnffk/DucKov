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

	enum PLAYER_STATE
	{
		IDLE = 0x00000001,
		RUN = 0x00000002,
		WALK = 0x00000004,
		RIGHT_WALK = 0x00000008,
		LEFT_WALK = 0x00000010,
		TPOSE = 0x00000020,
		HAND_UP = 0x00000040,
		HAND_UP_AND_RUN = 0x00000080,
		HAND_UP_AND_WALK = 0x00000100,
		HAND_UP_AND_RIGHT = 0x00000200,
		HAND_UP_AND_LEFT = 0x00000400,
		ROLL = 0x00000800,
		HIT = 0x00001000,
		HIT_REVERSE = 0x00002000,

		END = 0xffffffff
	};

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
private:
	uint32_t			m_iState = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	static unique_ptr<Player> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END