#include "Picking.h"
#include "GameInstance.h"

Picking::Picking(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
}

Picking::~Picking()
{
}

HRESULT Picking::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;

	_float2		vViewportDesc = CGameInstance::Get().Get_ViewportSize();

	D3D11_TEXTURE2D_DESC            TextureDesc{};
	TextureDesc.Width = vViewportDesc.x;
	TextureDesc.Height = vViewportDesc.y;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;


	TextureDesc.Usage = D3D11_USAGE_STAGING;
	TextureDesc.BindFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	m_pPixels = unique_ptr<_float4[]>(new _float4[vViewportDesc.x * vViewportDesc.y]);

	return S_OK;
}

void Picking::Update()
{
	_float2		vViewportDesc = CGameInstance::Get().Get_ViewportSize();

	if (FAILED(CGameInstance::Get().Copy_RenderTarget(TEXT("Target_PickPos"), m_pTexture2D)))
		return;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pTexture2D.Get(), 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

	memcpy(m_pPixels.get(), SubResource.pData, sizeof(_float4) * vViewportDesc.x * vViewportDesc.y);

	m_pContext->Unmap(m_pTexture2D.Get(), 0);
}

_bool Picking::Picking_to_Shader(_float4* pOut)
{
	_float2		vViewportDesc = CGameInstance::Get().Get_ViewportSize();

	::POINT			ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	/* 마우스 커서가 올라가있는 픽셀(1x1짜리 타일) 의 인덱스를 구하자. */
	uint32_t		iPixelIndex = ptMouse.y * static_cast<uint32_t>(vViewportDesc.x) + ptMouse.x;


	if (vViewportDesc.x * vViewportDesc.y < iPixelIndex) {
		return false;
	}

	if (0.f == m_pPixels[iPixelIndex].w)
		return false;

	*pOut = m_pPixels[iPixelIndex];

	return true;
}

unique_ptr<Picking> Picking::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, HWND hWnd)
{
	auto		pInstance = unique_ptr<Picking>(new Picking(pDevice, pContext));

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX("Failed to Created : Picking");
		return nullptr;
	}

	return pInstance;
}

