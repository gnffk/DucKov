#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Navigation final : public Component
{
public:
	typedef struct tagNavigationDesc
	{
		int32_t		iCurrentCellIndex = { -1 };
	}NAVIGATION_DESC;
private:
	Navigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~Navigation();

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath, const _tchar* pNavigationNeighborsFilePath);
	virtual HRESULT Initialize(void* pArg) override;
public:
	_vector SetUp_OnNavigation(_fvector vPosition);

public:
	HRESULT Ready_Neighbors();
	HRESULT Ready_Neighbors(const _tchar* pNeighbors);
	_bool isMove(_fvector vResultPos);

public:
	_vector Get_BlockingNormal(_fvector vPosition);
public:
	int32_t Find_CellIndex(_fvector vPosition);
	_bool Set_CurrentCell(_fvector vPosition);

#ifdef _DEBUG
public:
	HRESULT Render();

#endif

private:
	int32_t									m_iCurrentCellIndex = { -1 };
	vector<shared_ptr<class Cell>>			m_Cells;


#ifdef _DEBUG
private:
	shared_ptr<class Shader>				m_pShader = { nullptr };
#endif
public:
	static unique_ptr<Navigation> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pNavigationDataFilePath, const _tchar* pNavigationNeighborsFilePath = nullptr);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END