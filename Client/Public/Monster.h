#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"


NS_BEGIN(Engine)
class VIBuffer_Fbx;
class VIBuffer_Mesh;
class VIBuffer_Rect;
class Model;
class Shader;
class BaseCollider;
NS_END

NS_BEGIN(Client)
// 
class Monster : public ContainerObject
{
public:
	typedef struct tagTestModelDesc : public ContainerObject::GAMEOBJECT_DESC
	{
		uint32_t		iData;
	
	}TestModelDec;

protected:
	Monster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Monster(const Monster& Prototype);
public:
	virtual ~Monster();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Ready_Components();

	HRESULT Set_Model(wstring m_strPrototypeBaseName);

protected:
	uint32_t			m_iData = {};
	_wstring			m_ModelComponentName;

protected:


	shared_ptr<Model>			m_pModelCom = { nullptr };
	shared_ptr<Shader>			m_pShaderCom = { nullptr };



public:

	virtual shared_ptr<Prototype> Clone(void* pArg) override;

	void IMGUITEST();
	
};

NS_END