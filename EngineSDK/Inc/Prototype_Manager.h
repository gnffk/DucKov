#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL Prototype_Manager final
{
private:
	Prototype_Manager();

public:
	~Prototype_Manager() = default;

public:
	HRESULT	Initialize(uint32_t iNumLevels);
	HRESULT Add_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, std::any);

private:
	uint32_t m_iNumLevels{};

private:
//	unique_ptr<map<const _wstring, unique_ptr<void>>[]> m_pPrototypes{ nullptr };
	typedef	map<const _wstring, std::any> 		PROTOTYPES;
	unique_ptr<PROTOTYPES[]> m_pPrototypes{ nullptr };


private:
	std::any Find_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag);

public:
	static unique_ptr<Prototype_Manager> Create(uint32_t iNumLevels);

};

NS_END
