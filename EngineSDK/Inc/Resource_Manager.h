#pragma once

#include "Resource.h"

NS_BEGIN(Engine)

class Resource_Manager final
{
private:
	Resource_Manager();

public:
	~Resource_Manager();


public:
	HRESULT Initialize(ComPtr<ID3D11Device>p_Device, ComPtr<ID3D11DeviceContext>p_DeviceContext);

	shared_ptr<Resource> Find_Resource(uint32_t num, wstring key);
	HRESULT Add_Resource(ERESOURCE num, wstring key, shared_ptr<class Resource> pResource);
	void    Clear();
private:
	vector<unordered_map<wstring, shared_ptr<class Resource>>> m_ResourceMap;
private:
	ComPtr<ID3D11Device>			m_pDevice{ nullptr };
	ComPtr<ID3D11DeviceContext>		m_pDeviceContext{ nullptr };
public:
	static unique_ptr<Resource_Manager> Create(ComPtr<ID3D11Device>p_Device, ComPtr<ID3D11DeviceContext>p_DeviceContext);

};

NS_END
