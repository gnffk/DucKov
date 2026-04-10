#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Fbx.h"
#include "BaseShaderDX11.h"
NS_BEGIN(Client)

class TestModel final : public GameObject
{
public:
	typedef struct tagTestModelDesc : public GameObject::GAMEOBJECT_DESC
	{
		uint32_t		iData;
	}TestModelDec;

private:
	TestModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	TestModel(const TestModel& Prototype);
public:
	virtual ~TestModel();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	uint32_t			m_iData = {};

	shared_ptr<Engine::VIBuffer_Fbx>	m_pViBuffer = { nullptr };
	shared_ptr<Engine::BaseShaderDX11>	m_pShader = { nullptr };
public:
	static unique_ptr<TestModel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END