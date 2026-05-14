#include "Terrain.h"

#include "GameInstance.h"

Terrain::Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

Terrain::Terrain(const Terrain& Prototype)
	: GameObject{ Prototype }
{
}

Terrain::~Terrain()
{
	int a = 10;
}


HRESULT Terrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Terrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void Terrain::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void Terrain::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void Terrain::Late_Update(_float fTimeDelta)
{

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Terrain));

	__super::Late_Update(fTimeDelta);
}

HRESULT Terrain::Render()
{

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

	_float4x4 World = m_pTransformCom->GetWorldMatrix();


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &World, sizeof(_float4))))
		return E_FAIL;


	// render
	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT Terrain::Ready_Components()
{
	m_pVIBufferCom = dynamic_pointer_cast<VIBuffer_Terrain>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_VIBuffer_Terrain")));
	if (FAILED(__super::Add_Component(TEXT("Com_VIBuffer"), m_pVIBufferCom)))
		return E_FAIL;

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Com_Shader_Vtx_Terrian")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;


	return S_OK;
}

unique_ptr<Terrain> Terrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Terrain>(new Terrain(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Terrain");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Terrain::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Terrain(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Terrain");
		return nullptr;
	}

	return pInstance;
}

