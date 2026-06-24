#pragma once

#include "Component.h"
#include "GameObject.h"
enum class ColliderType {
	Sphere,
	AABB,
	OBB
};

enum class ColliderColor {
	RED,
	GREEN,
	BLACK
};

NS_BEGIN(Engine);

class ENGINE_DLL BaseCollider : public Component
{
protected:
	BaseCollider(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~BaseCollider();


public:
	int32_t Get_Tag() const { return m_sTag; }
	void Set_Tag(int32_t svTag) { m_sTag = (svTag); }

public:
	virtual HRESULT Initialize_Prototype() ;
	virtual HRESULT Initialize(void* pArg) ;
public:

	virtual HRESULT Intersect(_vector vPos, _vector vDir, float& pOutDist) = 0;


public:
	virtual void Update(float Timedelta) = 0;
	virtual HRESULT Bind_Resources();
	virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> m_batch);
	void SetOwner(GameObject* _owner) {
		m_Owner = _owner;
	}
	GameObject* GetOwner() {
		return m_Owner;
	}
	void SetColliderColor(ColliderColor _eColor) { m_eColor = _eColor; }
	ColliderColor GetCollor() { return m_eColor; }


	_float3 Get_Extend() { return m_Extend; }
	_float3 Get_Center() { return m_Center; }

	void Set_Extend(const _float3& extend) { m_Extend = extend; }
	void Set_Center(const _float3& center) { m_Center = center; }


	wstring& Get_GroupTag() { return m_GroupTag; }
	void	 Set_GroupTag(wstring _GroupTag) { m_GroupTag = _GroupTag; }
public:
	void	GUI_ColliderExtend();
protected:
	ColliderType m_eColliderID;
	ColliderColor m_eColor;
	int32_t m_sTag;
	wstring m_GroupTag;
	GameObject* m_Owner{ nullptr };
	_float4 m_color;
protected:
	_float3	m_Extend{};
	_float3	m_Center{};
protected:
	bool m_bRenderInitialized;

	

public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;
};

NS_END;