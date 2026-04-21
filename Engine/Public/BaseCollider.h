#pragma once

#include "Component.h"
#include "GameObject.h"
enum class ColliderType {
	Sphere,
	AABB,
	OBB
};

NS_BEGIN(Engine);

class ENGINE_DLL BaseCollider : public Component
{
protected:
	BaseCollider(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~BaseCollider();

public:
	void Set_OriginalColor(_ulong color) { m_dwOriginalColor = color; }
	void Set_IntersectColor(_ulong color) { m_dwIntersectColor = color; }
	void Set_CurrentColorIntersect() { m_dwCurrentColor = m_dwIntersectColor; };
	void Set_CurrentColorOriginal() { m_dwCurrentColor = m_dwOriginalColor; };

public:
	wstring Get_Tag() const { return m_sTag; }
	void Set_Tag(wstring svTag) { m_sTag = (svTag); }

public:
	virtual HRESULT Initialize_Prototype() ;
	virtual HRESULT Initialize(void* pArg) ;
public:
	virtual HRESULT Intersect(BaseCollider* pCollider) = 0 ;
	virtual HRESULT Intersect(_vector vPos, _vector vDir, float& pOutDist) = 0;


public:
	virtual void Update(float Timedelta) = 0;
	virtual HRESULT Bind_Resources();
	virtual HRESULT Render();

protected:
	ColliderType m_eColliderID;
	wstring m_sTag;



protected:
	bool m_bRenderInitialized;

	_ulong m_dwOriginalColor;
	_ulong m_dwIntersectColor;
	_ulong m_dwCurrentColor;



protected:
	ComPtr<ID3D11Buffer>			m_pVB = { nullptr };
	ComPtr<ID3D11Buffer>			m_pIB = { nullptr };

	UINT m_iNumVertexBuffers = 1;
	UINT m_iNumVertices = 0;
	UINT m_iVertexStride = sizeof(VTXTEX);

	UINT m_iNumIndices = 0;
	UINT m_iIndexStride = sizeof(uint16_t);
	DXGI_FORMAT m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	shared_ptr<vector<VTXTEX>> vertices;
	shared_ptr<vector<uint16_t>> indices;


public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;
};

NS_END;