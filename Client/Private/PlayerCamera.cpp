
#include "PlayerCamera.h"
#include "GameInstance.h"

PlayerCamera::PlayerCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: Camera{ pDevice, pContext }

{
}

PlayerCamera::PlayerCamera(const PlayerCamera& Prototype)
	: Camera{ Prototype }
{
}

PlayerCamera::~PlayerCamera()
{

}


HRESULT PlayerCamera::Initialize_Prototype()
{

	return S_OK;
}

HRESULT PlayerCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	GAMEOBJECT_DESC* pDesc =
		static_cast<GAMEOBJECT_DESC*>(pArg);
	m_iCameraType = pDesc->pCameraType;



	m_Owner = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player").get();



	_vector vPlayerPos =m_Owner->GetTransform()->Get_State(STATE::POSITION);

	_vector vOffset =XMVectorSet( 2.5f, 8.7f, -5.f ,0.f);

	_vector vCamPos =vPlayerPos + vOffset;

	_vector vLook =XMVector3Normalize(vPlayerPos - vCamPos);

	_vector vRight =XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f),vLook));

	_vector vUp =XMVector3Normalize(XMVector3Cross(vLook,vRight));

	m_pTransformCom->Set_State(STATE::LOOK,vLook);

	m_pTransformCom->Set_State(STATE::RIGHT,vRight);

	m_pTransformCom->Set_State(STATE::UP,vUp);


	return S_OK;
}

void PlayerCamera::Priority_Update(_float fTimeDelta)
{
	if (m_Owner == nullptr)
		return;

}

void PlayerCamera::Update(_float fTimeDelta)
{

	FollowCamera();
	__super::Set_View();



}

void PlayerCamera::Late_Update(_float fTimeDelta)
{


}

HRESULT PlayerCamera::Render()
{

	return S_OK;
}

HRESULT PlayerCamera::Ready_Components()
{
	__super::Clear_Compnent();

	return S_OK;
}

unique_ptr<PlayerCamera> PlayerCamera::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<PlayerCamera>(new PlayerCamera(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : PlayerCamera");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> PlayerCamera::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new PlayerCamera(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : PlayerCamera");
		return nullptr;
	}

	return pInstance;
}


void PlayerCamera::FollowCamera()
{
	if (m_Owner == nullptr)
		return;
	_vector vCamCurPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vPlayerPos = m_Owner->GetTransform()->Get_State(STATE::POSITION);

	POINT pt{};

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	float width = (float)g_iWinSizeX;

	float height = (float)g_iWinSizeY;

	float mouseX = (pt.x - width * 0.5f) / (width * 0.5f);

	float mouseY = (pt.y - height * 0.5f) / (height * 0.5f);

	float offsetX = mouseX * 2.f;

	float offsetZ = mouseY * 1.f;

	_vector vOffset = XMVectorSet(2.5f + offsetX, 8.7f , -5.f - offsetZ, 0.f);

	_vector vCamPos = vPlayerPos + vOffset;

	m_pTransformCom->Set_State(STATE::POSITION, vCamPos);
}
void PlayerCamera::Rotate(_float fDeltaX, _float fDeltaY, _float fTimeDelta)
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

void PlayerCamera::Zoom(_float fAmount)
{
	_vector vLook =
		XMVector3Normalize(
			m_pTransformCom->Get_State(STATE::LOOK));

	_vector vPos =
		m_pTransformCom->Get_State(STATE::POSITION);

	vPos += vLook * fAmount;

	m_pTransformCom->Set_State(
		STATE::POSITION,
		vPos);
}