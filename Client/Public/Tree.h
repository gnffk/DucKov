#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


NS_BEGIN(Engine)
class VIBuffer_Mesh;
class InstanceModel;
class Shader;
NS_END

NS_BEGIN(Client)
// 
class Tree final : public GameObject
{
public:


private:
	Tree(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Tree(const Tree& Prototype);
public:
	virtual ~Tree();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Ready_Components();


private:
	uint32_t			m_iData = {};

	_wstring			m_ModelComponentName;

private:
	shared_ptr<InstanceModel> m_pModelCom = nullptr;
	shared_ptr<Shader>        m_pShaderCom = nullptr;

public:
	HRESULT Add_Tree(const _float3& vPosition);
	HRESULT Add_Tree(const _float3& vPosition, const _float3& vScale, _float fYaw);
	HRESULT Erase_Tree(const _float3& vCenter, _float fRadius);
	HRESULT Clear_Trees();

	uint32_t Get_TreeCount() const;


public:
	static unique_ptr<Tree> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END