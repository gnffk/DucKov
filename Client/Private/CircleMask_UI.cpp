#include "CircleMask_UI.h"
#include "GameInstance.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

NS_BEGIN(Client)

CircleMask_UI::CircleMask_UI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: UIObject{ pDevice, pContext }
{
}

CircleMask_UI::CircleMask_UI(const CircleMask_UI& Prototype)
	: UIObject{ Prototype }
{
}

CircleMask_UI::~CircleMask_UI()
{
}

HRESULT CircleMask_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CircleMask_UI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CIRCLEMASK_DESC* pDesc = static_cast<CIRCLEMASK_DESC*>(pArg);

	if (pDesc != nullptr)
	{
		m_fMaskRadius = pDesc->fStartRadius;
		m_fMaxRadius = pDesc->fMaxRadius;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(640.f, 360.f, 1.f, 1.f));
	return S_OK;
}

void CircleMask_UI::Update(_float fTimeDelta)
{
	
}

HRESULT CircleMask_UI::Render()
{
    if (!m_bVisible)
        return S_OK;

    if (m_pShaderCom == nullptr || m_pVIBufferCom == nullptr)
        return E_FAIL;

    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float4x4 WorldMatrix{};
    _float4x4 ViewMatrix{};
    _float4x4 ProjMatrix{};

    // 화면 전체 크기
    XMStoreFloat4x4(
        &WorldMatrix,
        XMMatrixScaling(vViewportSize.x, vViewportSize.y, 1.f)
    );

    XMStoreFloat4x4(
        &ViewMatrix,
        XMMatrixIdentity()
    );

    XMStoreFloat4x4(
        &ProjMatrix,
        XMMatrixOrthographicLH(
            vViewportSize.x,
            vViewportSize.y,
            0.f,
            1.f
        )
    );

    _float fAspect = vViewportSize.x / vViewportSize.y;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskRadius", &m_fMaskRadius, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskSoftness", &m_fMaskSoftness, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAspect", &fAspect, sizeof(_float))))
        return E_FAIL;

    _float4 vMaskColor = { 0.f, 0.f, 0.f, 1.f };

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskColor", &vMaskColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CircleMask_UI::Ready_Components()
{
	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::NEVER),TEXT("Prototype_Com_Shader_CircleMask")));

	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	m_pVIBufferCom = dynamic_pointer_cast<VIBuffer_Rect>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::NEVER),TEXT("Prototype_Com_VIBuffer_Rect")));

	if (m_pVIBufferCom == nullptr)
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Com_VIBuffer"), m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

unique_ptr<CircleMask_UI> CircleMask_UI::Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CircleMask_UI>(new CircleMask_UI(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CircleMask_UI");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> CircleMask_UI::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new CircleMask_UI(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CircleMask_UI");
		return nullptr;
	}

	return pInstance;
}

NS_END