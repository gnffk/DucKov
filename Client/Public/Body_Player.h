#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class Shader;
class Model;
class BaseCollider;
NS_END

NS_BEGIN(Client)

class Body_Player final : public PartObject
{
public:
	typedef struct tagBodyPlayerDesc : public PartObject::PARTOBJECT_DESC
	{
		const uint32_t* pParentState = { nullptr };
	}BODY_PLAYER_DESC;

private:
	Body_Player(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Body_Player(const Body_Player& Prototype);
public:
	virtual ~Body_Player();

public:
	const _float4x4* Get_SocketMatrixPtr(const _char* pSocketName);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	shared_ptr<Model>			m_pModelCom = { nullptr };
	shared_ptr<Shader>			m_pShaderCom = { nullptr };
	shared_ptr<BaseCollider>	m_pOBBCom = { nullptr };

private:
	const uint32_t* m_pParentState = { nullptr };
	uint32_t nextAnim = 0;
	float    nextAnimPlus{1};
private:
	HRESULT Ready_Components();

public:
	static unique_ptr<Body_Player> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END