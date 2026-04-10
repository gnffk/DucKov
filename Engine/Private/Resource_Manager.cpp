#include "Resource_Manager.h"
#include "Resource.h"
#include "BaseShaderDX11.h"
// resize를 하면 그 과정에서 unordered_map의 복사 생성자를 보려고 함
//이게 이미 원소가 있는 상태에서 재할당(reallocate) 이 걸리면,
//기존 원소들을 새 메모리로 옮겨야 하는데,
//그 과정에서 구현체가 복사 경로를 잡아버리면 unique_ptr 때문에 막힌다.
Resource_Manager::Resource_Manager() 
{
}
Resource_Manager::~Resource_Manager()
{
}

HRESULT Resource_Manager::Initialize(ComPtr<ID3D11Device>p_Device, ComPtr<ID3D11DeviceContext>p_DeviceContext) {
	m_pDevice = p_Device;
	m_pDeviceContext = p_DeviceContext;
	m_ResourceMap.resize(ETOUI(ERESOURCE::END));


	return S_OK;

}

void Resource_Manager::Clear() {

	for (auto& map : m_ResourceMap)
	{
		for (auto& Pair : map) {
			Pair.second.reset();
		}
	}

	for (auto& map : m_ResourceMap) {


		map.clear();
		
	}
	
}



shared_ptr<Resource> Resource_Manager::Find_Resource(uint32_t num, wstring key)
{

	auto& iter = m_ResourceMap[num];


	if (iter.find(key) == m_ResourceMap[num].end())
		return nullptr;

	return iter.find(key)->second;

}

HRESULT Resource_Manager::Add_Resource(ERESOURCE num, wstring key, shared_ptr<Resource> pResource)
{

	auto& map = m_ResourceMap[ETOUI(num)];

	if (map.find(key) != map.end())
		return E_FAIL;

	map.emplace(key, std::move(pResource));
	return S_OK;

}

unique_ptr<Resource_Manager> Resource_Manager::Create(ComPtr<ID3D11Device>p_Device, ComPtr<ID3D11DeviceContext>p_DeviceContext)
{
	auto		pInstance = unique_ptr<Resource_Manager>(new Resource_Manager());

	if (FAILED(pInstance->Initialize(p_Device, p_DeviceContext)))
	{
		MSG_BOX("Failed to Created : Resource_Manager");

	}

	return pInstance;
}
