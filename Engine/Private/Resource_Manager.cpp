#include "Resource_Manager.h"

Resource_Manager::~Resource_Manager()
{
}

HRESULT Resource_Manager::Initialize() {

	return S_OK;
}

unique_ptr<Resource_Manager> Resource_Manager::Create()
{
	auto		pInstance = unique_ptr<Resource_Manager>(new Resource_Manager());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Resource_Manager");

	}

	return pInstance;
}
