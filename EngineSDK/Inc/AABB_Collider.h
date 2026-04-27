#pragma once
#include "BaseCollider.h"


NS_BEGIN(Engine);

class ENGINE_DLL AABB_Collider : public BaseCollider
{
protected:
	AABB_Collider(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~AABB_Collider();


public:
	virtual HRESULT Intersect(BaseCollider* pCollider);
	virtual HRESULT Intersect(_vector vPos, _vector vDir, float& pOutDist);

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Update(float Timedelta);
	virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> m_batch);

private:
	BoundingBox m_boudingBox;
	XMFLOAT4 m_color;

public:
	static unique_ptr<AABB_Collider> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) ;
};

NS_END;