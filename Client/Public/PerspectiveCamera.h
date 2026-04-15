#pragma once
#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Client)

class PerspectiveCamera final : public Camera
{

private:
	PerspectiveCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	PerspectiveCamera(const PerspectiveCamera& Prototype);
public:
	virtual ~PerspectiveCamera();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Ready_Components();
	void	KeyTestInput(_float fTimeDelta);
	void	Rotate(_float fDeltaX, _float fDeltaY);
private:
	bool  m_bMouseLook = false;
	POINT m_OldCursorPos{};
public:
	static unique_ptr<PerspectiveCamera> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END
