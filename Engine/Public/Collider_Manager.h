#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class Collider_Manager final
{
private:
	Collider_Manager() = default;

public:
	~Collider_Manager();

public:
	HRESULT Initialize(ComPtr<ID3D11Device> p_Device, ComPtr<ID3D11DeviceContext> p_DeviceContext);

	HRESULT Add_Collider(wstring GroupTag, class BaseCollider* pCollider);
	std::vector<BaseCollider*>* GetColliderGroups(wstring GroupTag) {
		auto iter = m_Colliders.find(GroupTag);
		if (iter == m_Colliders.end())
			return nullptr;

		return &iter->second;
	}
	std::unordered_map<wstring, std::vector<BaseCollider*>>& GetAllCollders() {
		return m_Colliders;
	}
	
public:
	HRESULT Find_Collider(wstring GroupTag, class BaseCollider* pCollider);
	void Update(float Timedelta);
	void Render();
	HRESULT Clear();
private:

	// 계속 빠르게 순회를 해야하기 떄문에 vector로 //  썼으면 비교하고 싶은 그룹을 바로 찾기 위해 hash 기반 unordered_map으로
	// 중간에 삭제될 Object들은 어짜피 Render에서 마지막 생명주기를 체크하고 있기 때문에 Collider을 GameObject Render앞에만 해주면 된다.
	std::unordered_map<wstring, std::vector<BaseCollider*>> m_Colliders;

	ComPtr<ID3D11Device> m_pDevice;
	ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	shared_ptr<PrimitiveBatch<VertexPositionColor>> m_batch;
	shared_ptr<BasicEffect> m_effect;
	ComPtr<ID3D11InputLayout> m_inputLayout;

public:
	static unique_ptr<Collider_Manager> Create(ComPtr<ID3D11Device> p_Device, ComPtr<ID3D11DeviceContext> p_DeviceContext);

};

NS_END