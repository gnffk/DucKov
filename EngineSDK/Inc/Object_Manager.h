#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class Object_Manager final
{
private:
	Object_Manager() = default;

public:
	~Object_Manager();

public:
	HRESULT Initialize(uint32_t iNumLevels);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	void Clear(uint32_t iClearLevelIndex);
public:

	HRESULT Add_GameObject_toLayer(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag,
		uint32_t iLayerLevelIndex, const _wstring& strLayerTag, void* pArg);

	shared_ptr<GameObject> Find_Object(uint32_t iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strObjectTag);
private:
	uint32_t	m_iNumLevels = {};
private:
	unique_ptr<map<const _wstring, unique_ptr<class Layer>>[]>		m_pLayers = { nullptr };
	typedef map<const _wstring, unique_ptr<class Layer>>		LAYERS;


private:
	class Layer* Find_Layer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag);

public:
	static unique_ptr<Object_Manager> Create(uint32_t iNumLevels);

};

NS_END