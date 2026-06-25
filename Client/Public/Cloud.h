#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class Model;
class Shader;
NS_END

NS_BEGIN(Client)

class Cloud final : public GameObject
{
public:
	typedef struct tagCloudDesc : public GameObject::GAMEOBJECT_DESC
	{
		_float3 vSpawnPos = { 0.f, 0.f, 0.f };
		// 왼쪽 이동 속도
		_float fMoveSpeed = 2.f;

		_float fResetDistance = 100.f;

	} CLOUD_DESC;

private:
	Cloud(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Cloud(const Cloud& Prototype);

public:
	virtual ~Cloud();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	HRESULT Ready_Components();

private:
	shared_ptr<Model>		m_pModelCom = nullptr;
	shared_ptr<Shader>		m_pShaderCom = nullptr;

private:
	_wstring				m_ModelComponentName = L"";

private:
	_float3					m_vStartPos = {};
	_float					m_fMoveSpeed = 2.f;
	_float					m_fResetDistance = 50.f;

public:
	static unique_ptr<Cloud> Create(
		ComPtr<ID3D11Device> pDevice,
		ComPtr<ID3D11DeviceContext> pContext
	);

	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END