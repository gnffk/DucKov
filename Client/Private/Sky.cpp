#include "Sky.h"

#include "GameInstance.h"

Sky::Sky(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

Sky::Sky(const Sky& Prototype)
	: GameObject{ Prototype }
{
}

Sky::~Sky()
{

}


HRESULT Sky::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Sky::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void Sky::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void Sky::Update(_float fTimeDelta)
{

	_float4 position;
	CGameInstance::Get().Get_MainCameraPosition(position);
	m_pTransformCom->Set_State(STATE::POSITION,
		XMLoadFloat4(&position));

	__super::Update(fTimeDelta);
}

void Sky::Late_Update(_float fTimeDelta)
{

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::PRIORITY, SHARED_THIS(Sky));

	__super::Late_Update(fTimeDelta);
}

HRESULT Sky::Render()
{

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCamerwaProjectionMatrix( Proj);
	CGameInstance::Get().Get_MainCamerwaViewMatrix(View);

	_float4x4 World = m_pTransformCom->GetWorldMatrix();


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 2)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT Sky::Ready_Components()
{
	__super::Clear_Compnent();

	m_pVIBufferCom = dynamic_pointer_cast<VIBuffer_Cube>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_VIBuffer_Cube")));
	if (FAILED(__super::Add_Component(TEXT("Com_VIBuffer"), m_pVIBufferCom)))
		return E_FAIL;

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_VtxCube")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Sky")));
	if (FAILED(__super::Add_Component(TEXT("Com_Texture"), m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

unique_ptr<Sky> Sky::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Sky>(new Sky(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Sky");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Sky::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Sky(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Sky");
		return nullptr;
	}

	return pInstance;
}

