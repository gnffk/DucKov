
#include "PerformaceCamera.h"
#include "GameInstance.h"

PerformaceCamera::PerformaceCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: Camera{ pDevice, pContext }

{
}

PerformaceCamera::PerformaceCamera(const PerformaceCamera& Prototype)
	: Camera{ Prototype }
{
}

PerformaceCamera::~PerformaceCamera()
{

}


HRESULT PerformaceCamera::Initialize_Prototype()
{

	return S_OK;
}

HRESULT PerformaceCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
	m_iCameraType = pDesc->pCameraType;


	if(m_Owner != nullptr)
		m_pTransformCom->Set_State(STATE::POSITION, m_Owner->GetTransform()->Get_State(STATE::POSITION));

	return S_OK;
}

void PerformaceCamera::Priority_Update(_float fTimeDelta)
{

}

void PerformaceCamera::Update(_float fTimeDelta)
{

	__super::Set_View();

}

void PerformaceCamera::Late_Update(_float fTimeDelta)
{

}

HRESULT PerformaceCamera::Render()
{

	return S_OK;
}

HRESULT PerformaceCamera::Ready_Components()
{
	__super::Clear_Compnent();

	return S_OK;
}

unique_ptr<PerformaceCamera> PerformaceCamera::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<PerformaceCamera>(new PerformaceCamera(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : PerformaceCamera");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> PerformaceCamera::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new PerformaceCamera(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : PerformaceCamera");
		return nullptr;
	}

	return pInstance;
}


void PerformaceCamera::Rotate(_float fDeltaX, _float fDeltaY, _float fTimeDelta)
{
	if (nullptr == m_pTransformCom)
		return;


	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(STATE::UP));
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fDeltaX);
	//m_pTransformCom->Turn(vUp, fDeltaX);


	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT));
	m_pTransformCom->Turn(vRight, fDeltaY);

	GetCursorPos(&m_OldCursorPos);
}


void PerformaceCamera::Zoom(_float fAmount)
{
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));

	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

	vPos += vLook * fAmount;

	m_pTransformCom->Set_State(STATE::POSITION,vPos);
}