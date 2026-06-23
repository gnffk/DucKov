#pragma once
#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Client)

class PlayerCamera final : public Camera
{

private:
	PlayerCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	PlayerCamera(const PlayerCamera& Prototype);
public:
	virtual ~PlayerCamera();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Ready_Components();
	void   SetOwner(GameObject* _Owner) { m_Owner = _Owner; }
	void   FollowCamera();
	void	Rotate(_float fDeltaX, _float fDeltaY, _float fTimeDelta);
	void	Zoom(_float fAmount);
private:
	bool  m_bMouseLook = false;
	uint32_t m_iCameraType{};
	POINT m_OldCursorPos{};

private:
	_float3 m_vBaseOffset = { 2.5f, 8.7f, -5.f };

#ifdef _DEBUG
private:
	void IMGUI_CameraOffset();

#endif
	GameObject* m_Owner = nullptr;
public:
	static unique_ptr<PlayerCamera> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END
