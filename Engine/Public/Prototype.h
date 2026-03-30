#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL Prototype abstract
{
protected:
	Prototype();
public:
	virtual ~Prototype();

public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;
};

NS_END