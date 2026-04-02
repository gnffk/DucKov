#pragma once
#include "Engine_Defines.h"
#include "Resource.h"
NS_BEGIN(Engine)

class ENGINE_DLL Fbx_Resource final : public Resource
{
private:
	Fbx_Resource(wstring filename);

public:
	virtual ~Fbx_Resource();

public:
	virtual HRESULT Load();

private:
	wstring m_wsfilename;
};

NS_END
