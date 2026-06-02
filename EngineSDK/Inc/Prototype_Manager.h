#pragma once
#include "GameObject.h"
#include "Component.h"
#include "Shader.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Fbx.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Mesh.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Cell.h"
#include "Model.h"
#include "Texture.h"
#include "FSM.h"




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


	vector<wstring> Get_PrototypeNames(uint32_t levelIndex)
	{
		vector<wstring> names;

		PROTOTYPES& protos = m_pPrototypes[levelIndex];

		for (auto& pair : protos)
		{
			names.push_back(pair.first); 
		}

		return names;
	}


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
