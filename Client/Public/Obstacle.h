#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


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
class Obstacle final : public GameObject
{
public:


private:
	Obstacle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Obstacle(const Obstacle& Prototype);
public:
	virtual ~Obstacle();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	HRESULT Ready_Components();


private:
	uint32_t			m_iData = {};

	_wstring			m_ModelComponentName;
private:

	shared_ptr<Model>			m_pModelCom = { nullptr };
	shared_ptr<Shader>			m_pShaderCom = { nullptr };



public:
	static unique_ptr<Obstacle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

	void IMGUITEST();

};

NS_END