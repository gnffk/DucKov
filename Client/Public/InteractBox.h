

#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


NS_BEGIN(Engine)
class Model;
class Shader;
class BaseCollider;
NS_END

NS_BEGIN(Client)
// 
class InteractBox final : public GameObject
{
public:
	enum InteractType {
		StartBox,
		FoodBox,
		GeneralBox,
		GoodBox,
		Die,
		END

	};

public:
	typedef struct tagInteractBoxDesc : public GameObject::GAMEOBJECT_DESC
	{
		InteractType eInteractType = InteractType::GeneralBox;
		_float3 vSpawnPos = { 0.f, 0.f, 0.f };
	} INTERACTBOX_DESC;

private:
	InteractBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	InteractBox(const InteractBox& Prototype);
public:
	virtual ~InteractBox();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Ready_Components();
	HRESULT Ready_UI();

public:
	map<string, shared_ptr<class GameObject>> m_pUI;

private:
	uint32_t			m_iData = {};
	InteractType		m_eInteractType{InteractType::END};
	_wstring			m_ModelComponentName;
private:

	shared_ptr<Model>			m_pModelCom = { nullptr };
	shared_ptr<Shader>			m_pShaderCom = { nullptr };



public:
	static unique_ptr<InteractBox> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
	void IMGUITEST();


};

NS_END