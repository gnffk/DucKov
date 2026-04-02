#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL Resource abstract
{
protected:
	Resource();

public:
	virtual ~Resource() = default ;

	virtual HRESULT Load();
};

NS_END
