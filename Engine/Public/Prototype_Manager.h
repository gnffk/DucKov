#pragma once
#include "GameObject.h"
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Prototype_Manager final
{
private:
	Prototype_Manager();

public:
	~Prototype_Manager();

public:
	HRESULT	Initialize(uint32_t iNumLevels);
	HRESULT Add_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, unique_ptr<Prototype> pPrototype);
	shared_ptr<Prototype> Clone_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void Clear(uint32_t iClearLevelIndex);
private:
	uint32_t m_iNumLevels{};

private:
	unique_ptr<map<const _wstring, unique_ptr<Prototype>>[]>		m_pPrototypes = { nullptr };
	typedef map<const _wstring, unique_ptr<Prototype>>				PROTOTYPES;

private:
	Prototype* Find_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag);

public:
	static unique_ptr<Prototype_Manager> Create(uint32_t iNumLevels);

};

NS_END
