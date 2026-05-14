#include "Camera_Manager.h"

Camera_Manager::~Camera_Manager()
{
}


HRESULT Camera_Manager::Initialize() {


	return S_OK;
}


HRESULT Camera_Manager::Change_Camera(uint32_t iCameraType)
{
	auto camera = Find_Camera(iCameraType).lock();
	if (camera) {
		p_MainCamera = camera;
	}

	return S_OK;
}

HRESULT Camera_Manager::Add_Camera(uint32_t iCameraType, shared_ptr<Camera> pCamera)
{
	p_Cameras[iCameraType] = pCamera;
	return S_OK;

}

HRESULT Camera_Manager::Get_MainCameraMatrix( _float4x4& ViewMatrix, _float4x4& ProjectionMatrix)
{
	auto camera = p_MainCamera.lock();
	if (camera) {
		camera->GetShaderMatrix( ViewMatrix, ProjectionMatrix);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT Camera_Manager::Get_MainCameraWorldMatrix(_float4x4& WorldMatrix)
{
	auto camera = p_MainCamera.lock();
	if (camera) {
		camera->GetWorldMatrix(WorldMatrix);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT Camera_Manager::Get_MainCamerwaViewMatrix(_float4x4& ViewMatrix)
{
	auto camera = p_MainCamera.lock();
	if (camera) {
		camera->GetViewMatrix(ViewMatrix);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT Camera_Manager::Get_MainCamerwaProjectionMatrix(_float4x4& ProjectionMatrix)
{
	auto camera = p_MainCamera.lock();
	if (camera) {
		camera->GetProjectionMatrix(ProjectionMatrix);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT Camera_Manager::Get_MainCameraPosition(_float4& CameraPosition)
{
	auto camera = p_MainCamera.lock();
	if (camera) {
		camera->GetPosition(CameraPosition);
		return S_OK;
	}
	return E_FAIL;
}

weak_ptr<Camera> Camera_Manager::Find_Camera(uint32_t iCameraType)
{

	auto	iter = p_Cameras.find(iCameraType);
	if (iter == p_Cameras.end())
		return weak_ptr<Camera>();

	return iter->second;
}


unique_ptr<Camera_Manager> Camera_Manager::Create()
{
	auto		pInstance = unique_ptr<Camera_Manager>(new Camera_Manager());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Camera_Manager");
		return nullptr;
	}

	return pInstance;
}
void Camera_Manager::Clear() {
	p_MainCamera.reset();
}