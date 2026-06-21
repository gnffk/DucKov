

#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END


NS_BEGIN(Client)

class Player_Helmat final : public PartObject
{
public:
	typedef struct tagHelmatDesc : public PartObject::PARTOBJECT_DESC
	{
		const uint32_t* pParentState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
	}HELMAT_DESC;

private:
	Player_Helmat(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Player_Helmat(const Player_Helmat& Prototype);
public:
	virtual ~Player_Helmat();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void Set_HelmatType(const string& strHelmatType)
	{
		currentHelmatType = strHelmatType;
	}

	const string& Get_HelmatType() const
	{
		return currentHelmatType;
	}


private:
	map<string, shared_ptr<Model>>	m_pModelMapCom;
	shared_ptr<Shader>			m_pShaderCom = { nullptr };

private:
	const uint32_t* m_pParentState = { nullptr };
	const _float4x4* m_pSocketMatrix = { nullptr };


	string currentHelmatType = "Default";
private:
	HRESULT Ready_Components();

public:
	static unique_ptr<Player_Helmat> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END