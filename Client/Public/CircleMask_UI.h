#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CircleMask_UI final : public UIObject
{
public:
	typedef struct tagCircleMaskDesc : public UIObject::UIOBJECT_DESC
	{
		_float fStartRadius = -0.05f;
		_float fMaxRadius = 1.2f;
	} CIRCLEMASK_DESC;

private:
	CircleMask_UI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CircleMask_UI(const CircleMask_UI& Prototype);

public:
	virtual ~CircleMask_UI();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Radius(_float fRadius) { m_fMaskRadius = fRadius; }
	_float Get_Radius() const { return m_fMaskRadius; }

	void Set_Visible(_bool bVisible) { m_bVisible = bVisible; }
	_bool Is_Visible() const { return m_bVisible; }

private:
	HRESULT Ready_Components();

private:
	shared_ptr<Shader> m_pShaderCom = nullptr;
	shared_ptr<VIBuffer_Rect> m_pVIBufferCom = nullptr;

private:
	_float m_fMaskRadius = -0.05f;
	_float m_fMaskSoftness = 0.05f;
	_float m_fMaxRadius = 1.2f;
	_bool m_bVisible = true;

public:
	static unique_ptr<CircleMask_UI> Create(
		ComPtr<ID3D11Device> pDevice,
		ComPtr<ID3D11DeviceContext> pContext
	);

	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END