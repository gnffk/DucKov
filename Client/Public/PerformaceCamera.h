
#pragma once
#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Client)

class PerformaceCamera final : public Camera
{

private:
	PerformaceCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	PerformaceCamera(const PerformaceCamera& Prototype);
public:
	virtual ~PerformaceCamera();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Ready_Components();
	
	void	Rotate(_float fDeltaX, _float fDeltaY, _float fTimeDelta);
	void    Zoom(_float fAmount);
private:
	bool  m_bMouseLook = false;
	uint32_t m_iCameraType{};
	POINT m_OldCursorPos{};

	GameObject* m_Owner = nullptr;
public:
	static unique_ptr<PerformaceCamera> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END
