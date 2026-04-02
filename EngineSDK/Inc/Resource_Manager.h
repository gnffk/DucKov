#pragma once
#include "Engine_Defines.h"


NS_BEGIN(Engine)

class Resource_Manager final
{
private:
	Resource_Manager() = default;

public:
	~Resource_Manager();

public:
	HRESULT Initialize();

public:
	static unique_ptr<Resource_Manager> Create();

};

NS_END
