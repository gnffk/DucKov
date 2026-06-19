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
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<GameObject> pRenderObject);
	HRESULT Draw();



private:
	ComPtr<ID3D11Device>					m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>				m_pContext = { nullptr };
	list<shared_ptr<GameObject>>			m_RenderObjects[ETOUI(RENDERGROUP::END)];

private:
	shared_ptr<class VIBuffer_Rect>		m_pVIBuffer = { nullptr };
	shared_ptr<class Shader>				m_pShader = { nullptr };
	_float4x4								m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};


#ifdef _DEBUG
private:
	list<shared_ptr<Component>>			m_DebugComponents;


	int debugRender = 1;
#endif
	

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_Lights();
	HRESULT Render_Combined();
	HRESULT Render_NonLights();
	HRESULT Render_UI();
	HRESULT Render_Effect();

	HRESULT Render_Final();

	HRESULT Render_Scene();
	HRESULT Render_Combined_InScene();


	HRESULT Render_DEBUG();


private:


	HRESULT Render_BloomDownSamples();

	HRESULT Render_DownSample(const _wstring& strSrcTarget,const _wstring& strDstMRT,DEFERRED ePass,const _float2& vSrcTexelSize);


private:
	HRESULT Render_Blur1();
	HRESULT Render_Blur2();
	HRESULT Render_Blur3();

	HRESULT Render_BlurPass(const _wstring& strSrcTarget,const _wstring& strDstMRT,const _float2& vSrcTexelSize, int iHorizontal);
private:
	HRESULT Render_UpSample1();
	HRESULT Render_UpSample2();
	HRESULT Render_UpSamplePass(const _wstring& strSrcTarget,const _wstring& strDstMRT,const _float2& vSrcTexelSize);
	HRESULT Render_UpSampleBlend(const _wstring& strLowTarget, const _wstring& strBlendTarget, const _wstring& strDstMRT, const _float2& vLowTexelSize);



public:

	static unique_ptr<Renderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);


};

NS_END
