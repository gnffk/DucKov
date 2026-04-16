
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


	// ÀÌµ¿
 	if (GetAsyncKeyState('W') & 0x8000)
		m_pTransformCom->Go_Straight(fTimeDelta);

	if (GetAsyncKeyState('S') & 0x8000)
		m_pTransformCom->Go_Backward(fTimeDelta);

	if (GetAsyncKeyState('A') & 0x8000)
		m_pTransformCom->Go_Left(fTimeDelta);

	if (GetAsyncKeyState('D') & 0x8000)
		m_pTransformCom->Go_Right(fTimeDelta);

	if (GetAsyncKeyState('Q') & 0x8000)
		m_pTransformCom->Go_Up(fTimeDelta);

	if (GetAsyncKeyState('E') & 0x8000)
		m_pTransformCom->Go_Down(fTimeDelta);


	if (CGameInstance::Get().Mouse_Down(MOUSEKEYSTATE::DIM_RB)) {

		GetCursorPos(&m_OldCursorPos);
	}

	if (CGameInstance::Get().Mouse_Pressing(MOUSEKEYSTATE::DIM_RB)) {

		POINT curPos{};
		GetCursorPos(&curPos);

		float deltaX = static_cast<float>(curPos.x - m_OldCursorPos.x) / 10.f;
		float deltaY = static_cast<float>(curPos.y - m_OldCursorPos.y) / 10.f;

		Rotate(deltaX, deltaY);

	}


}

void PerspectiveCamera::Rotate(_float fDeltaX, _float fDeltaY)
{
	if (nullptr == m_pTransformCom)
		return;



	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), fDeltaX );


	//_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(STATE::UP));
	//m_pTransformCom->Rotation(vUp, fDeltaX);


	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT));
	m_pTransformCom->Rotation(vRight, fDeltaY );
}