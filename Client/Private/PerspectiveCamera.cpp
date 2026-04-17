
#include "PerspectiveCamera.h"
#include "GameInstance.h"

PerspectiveCamera::PerspectiveCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: Camera{ pDevice, pContext }

{
}

PerspectiveCamera::PerspectiveCamera(const PerspectiveCamera& Prototype)
	: Camera{ Prototype }
{
}

PerspectiveCamera::~PerspectiveCamera()
{

}


HRESULT PerspectiveCamera::Initialize_Prototype()
{

	return S_OK;
}

HRESULT PerspectiveCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	
	return S_OK;
}

void PerspectiveCamera::Priority_Update(_float fTimeDelta)
{
	
}

void PerspectiveCamera::Update(_float fTimeDelta)
{
	__super::Set_View();
	KeyTestInput(fTimeDelta);

}

void PerspectiveCamera::Late_Update(_float fTimeDelta)
{
	
}

HRESULT PerspectiveCamera::Render()
{

	return S_OK;
}

HRESULT PerspectiveCamera::Ready_Components()
{

	return S_OK;
}

unique_ptr<PerspectiveCamera> PerspectiveCamera::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<PerspectiveCamera>(new PerspectiveCamera(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : PerspectiveCamera");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> PerspectiveCamera::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new PerspectiveCamera(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : PerspectiveCamera");
		return nullptr;
	}

	return pInstance;
}

void PerspectiveCamera::KeyTestInput(_float fTimeDelta) {


	


	if (CGameInstance::Get().Mouse_Down(MOUSEKEYSTATE::DIM_RB)) {

		GetCursorPos(&m_OldCursorPos);
	}

	if (CGameInstance::Get().Mouse_Pressing(MOUSEKEYSTATE::DIM_RB)) {
		// ÀÌµ¿

		if (CGameInstance::Get().Key_Pressing(DIK_W))
			m_pTransformCom->Go_Straight(fTimeDelta);

		if (CGameInstance::Get().Key_Pressing(DIK_S))
			m_pTransformCom->Go_Backward(fTimeDelta);

		if (CGameInstance::Get().Key_Pressing(DIK_A))
			m_pTransformCom->Go_Static_Left(fTimeDelta);

		if (CGameInstance::Get().Key_Pressing(DIK_D))
			m_pTransformCom->Go_Static_Right(fTimeDelta);

		if (CGameInstance::Get().Key_Pressing(DIK_E))
			m_pTransformCom->Go_Up(fTimeDelta);

		if (CGameInstance::Get().Key_Pressing(DIK_Q))
			m_pTransformCom->Go_Down(fTimeDelta);
		POINT curPos{};
		GetCursorPos(&curPos);

		float deltaX = static_cast<float>(curPos.x - m_OldCursorPos.x);
		float deltaY = static_cast<float>(curPos.y - m_OldCursorPos.y) ;
		



		Rotate(deltaX, deltaY, fTimeDelta);
		
	}
	


}

void PerspectiveCamera::Rotate(_float fDeltaX, _float fDeltaY, _float fTimeDelta)
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