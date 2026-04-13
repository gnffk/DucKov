#pragma once

#include "Component.h"

/* Vertex + Index Buffer */

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer abstract : public Component
{
protected:
	VIBuffer(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~VIBuffer();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bind_Resources();
	virtual HRESULT Render();


protected:
	shared_ptr <vector<Mesh_VIBUFFER>> Meshes_VIBuffers;


public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;

};

NS_END