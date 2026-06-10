#include "UIObject.h"
#include "GameInstance.h"

UIObject::UIObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }
{
}

UIObject::~UIObject()
{
}

HRESULT UIObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT UIObject::Initialize(void* pArg)
{
	auto		pDesc = static_cast<UIOBJECT_DESC*>(pArg);

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_float2		vViewportSize = CGameInstance::Get().Get_ViewportSize();

	Update_Transform();

	XMStoreFloat4x4(&m_ViewMatrix,XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix,XMMatrixOrthographicLH(vViewportSize.x, vViewportSize.y, 0.f, 1.f));

	return S_OK;
}

void UIObject::Priority_Update(_float fTimeDelta)
{
}

void UIObject::Update(_float fTimeDelta)
{
}

void UIObject::Late_Update(_float fTimeDelta)
{
	Update_Transform();
}

HRESULT UIObject::Render()
{
	return S_OK;
}

void UIObject::Update_Transform()
{
	_float2		vViewportSize = CGameInstance::Get().Get_ViewportSize();

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION,XMVectorSet(m_fX - vViewportSize.x * 0.5f, -m_fY + vViewportSize.y * 0.5f, 0.f, 1.f));
}
