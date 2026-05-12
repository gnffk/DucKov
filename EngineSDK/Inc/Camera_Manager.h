#pragma once

#include "Camera.h"

NS_BEGIN(Engine)

class Camera_Manager final
{
private:
	Camera_Manager() = default;

public:
	~Camera_Manager();

public:
	HRESULT Initialize();

	HRESULT Change_Camera(uint32_t iCameraType);
	HRESULT Add_Camera(uint32_t iCameraType, shared_ptr<Camera> pCamera);


	HRESULT Get_MainCameraMatrix( _float4x4& ViewMatrix  , _float4x4& ProjectionMatrix);
	HRESULT Get_MainCameraWorldMatrix(_float4x4& WorldMatrix);
	HRESULT Get_MainCamerwaViewMatrix(_float4x4& ViewMatrix);
	HRESULT Get_MainCamerwaProjectionMatrix(_float4x4& ProjectionMatrix);
	weak_ptr<Camera> Find_Camera(uint32_t iCameraType);
	

	void	Clear();
private:
	map<uint32_t , weak_ptr<Camera>> p_Cameras;
	weak_ptr<Camera> p_MainCamera;
public:
	static unique_ptr<Camera_Manager> Create();

};

NS_END