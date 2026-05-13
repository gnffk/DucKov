#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL Map_Manager final
{
private:
	Map_Manager();

public:
	~Map_Manager();

public:
	HRESULT	Initialize();
	HRESULT Save(string _mapDataName, bool _overwrite);
	HRESULT Load(string _mapDataName, uint32_t Levelindex);
	vector<string>& GetMapNames(){
		return m_MapNames;
	}
	HRESULT BinFileCheck();
	void Clear();


public:
	string WStringToString(const std::wstring& wstr);
	wstring StringToWString(const string& str);
private:
	map<string, nlohmann::json> m_mapJsonFiles;
	vector<string> m_MapNames;
public:
	static unique_ptr<Map_Manager> Create();

};

NS_END
