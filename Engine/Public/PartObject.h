#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL PartObject abstract : public GameObject
{
public:
	typedef struct tagPartObjectDesc : public GameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix;
	}PARTOBJECT_DESC;

protected:
	PartObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	PartObject(const PartObject& Prototype);
public:
	virtual ~PartObject();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	const _float4x4* m_pParentMatrix = { nullptr };
	_float4x4				m_CombinedWorldMatrix = { };

protected:
	void Make_CombinedWorldMatrix(_fmatrix ChildMatrix);
	HRESULT Bind_WorldMatrix(shared_ptr<class Shader> pShader, const _char* pConstantName);

public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;

};

NS_END