#pragma once
#include "GameObject.h"
#include "Shader.h"
NS_BEGIN(Engine)

class ENGINE_DLL Camera : public GameObject
{
protected:
	Camera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Camera(const Camera& Prototype);

public:
	virtual ~Camera();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Set_View();
	virtual void Set_Proj(float fov, float aspect, float nearZ, float farZ);

	void GetShaderMatrix( _float4x4& ViewMatrix, _float4x4& ProjectionMatrix);


protected:

	_float4x4		m_WorldMatrix{};
	_float4x4		m_ViewMatrix{};
	_float4x4		m_ProjectionMatrix{};

};

NS_END
