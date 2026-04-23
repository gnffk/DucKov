#include "Camera.h"

Camera::Camera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : GameObject{pDevice, pContext}
{
}

Camera::Camera(const Camera& Prototype) :
	GameObject{Prototype.m_pDevice,Prototype.m_pContext}
{

}

Camera::~Camera()
{
}

HRESULT Camera::Initialize_Prototype() {

	return S_OK;
}

HRESULT Camera::Initialize(void* pArg) {

	__super::Initialize(pArg);

	Set_Proj(60.f, 16.f / 9.f, 0.01f, 500.f);
	Set_View();

	return S_OK;
}
void Camera::Priority_Update(_float fTimeDelta) {

}
void Camera::Update(_float fTimeDelta) {
	
}
void Camera::Late_Update(_float fTimeDelta) {

}
HRESULT Camera::Render() {

	return S_OK;
}

void Camera::Set_View()
{
	_vector eye = m_pTransformCom->Get_State(STATE::POSITION);
	_vector look = m_pTransformCom->Get_State(STATE::LOOK);
	_vector up = m_pTransformCom->Get_State(STATE::UP);
	
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixLookToLH(eye, look, up));
}

void Camera::Set_Proj(float fov, float aspect, float nearZ, float farZ)
{
	XMStoreFloat4x4(&m_ProjectionMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), aspect, nearZ, farZ));

}

void Camera::GetShaderMatrix( _float4x4& ViewMatrix, _float4x4& ProjectionMatrix)
{
	

	ViewMatrix = m_ViewMatrix;
 	ProjectionMatrix = m_ProjectionMatrix;

}

void Camera::GetWorldMatrix(_float4x4& WorldMatrix) {
	WorldMatrix = m_WorldMatrix;
}