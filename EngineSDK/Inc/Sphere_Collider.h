#pragma once
#include "BaseCollider.h"


NS_BEGIN(Engine);

class ENGINE_DLL Sphere_Collider : public BaseCollider
{
protected:
	Sphere_Collider(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~Sphere_Collider();


public:

	virtual HRESULT Intersect(_vector vPos, _vector vDir, float& pOutDist);

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Update(float Timedelta);
	virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> m_batch);

	BoundingSphere Get_BoudingSphere() { return m_boudingSphere; }
private:


	BoundingSphere m_boudingSphere;

public:
	static unique_ptr<Sphere_Collider> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg);
};

NS_END;