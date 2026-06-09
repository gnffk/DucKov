#pragma once

#include "UIObject.h"

NS_BEGIN(Engine)

class UI_Manager final
{
private:
    UI_Manager() = default;

public:
    ~UI_Manager();

public:
    HRESULT Initialize();

    void Priority_Update(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Late_Update(_float fTimeDelta);
    HRESULT Render();

public:
    HRESULT Add_UIObject( const wstring& strUIGroup, shared_ptr<UIObject> pUIObject);

    vector<shared_ptr<UIObject>>* Find_UIGroup(const wstring& strUIGroup);

    shared_ptr<UIObject> Find_UIObject(const wstring& strUIGroup, const wstring& strUIObjectTag);

    void Set_MainUIGroup(const wstring& strUIGroup);
    const wstring& Get_MainUIGroup() const;

    void Clear();
    void Clear_Group(const wstring& strUIGroup);

private:
    map<wstring, vector<shared_ptr<UIObject>>> m_UIGroups;
    wstring m_strMainUIGroup = L"";

public:
    static unique_ptr<UI_Manager> Create();
};

NS_END