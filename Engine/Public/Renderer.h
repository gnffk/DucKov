#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class Renderer final
{
private:
	Renderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	~Renderer();

public:
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<GameObject> pRenderObject);
	HRESULT Draw();

private:
	list<shared_ptr<GameObject>>			m_RenderObjects[ETOUI(RENDERGROUP::END)];

public:
	static unique_ptr<Renderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};

NS_END
