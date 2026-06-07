#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class Layer final
{
private:
	Layer() = default;
public:
	~Layer() = default;

public:
	HRESULT Add_GameObject(shared_ptr<GameObject> pGameObject);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	list<shared_ptr<GameObject>>& Get_GameObjects(){ return m_GameObjects; }
	_bool Is_Empty() const{
		return m_GameObjects.empty();}

private:
	list<shared_ptr<GameObject>>			m_GameObjects;

	
public:
	static unique_ptr<Layer> Create();

};

NS_END