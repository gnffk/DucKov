#include "VIBuffer.h"



HRESULT VIBuffer::Initialize_Prototype()
{
    HRESULT hr;

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = static_cast<UINT>(sizeof(vertex2) * vertices.get()->size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = vertices->data();

    hr = m_pDevice->CreateBuffer(&vbd, &initData, VertexBuffer_.GetAddressOf());
    if (FAILED(hr)) {
        MSG_BOX("VertexBuffer_ FAILED");
    }

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = static_cast<UINT>(sizeof(INDEX32) * indices->size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;

    initData.pSysMem = indices->data();

    hr = m_pDevice->CreateBuffer(&ibd, &initData, IndexBuffer_.GetAddressOf());
    if (FAILED(hr)) {
        MSG_BOX("IndexBuffer_ FAILED");
   
    }
	return S_OK;
}

HRESULT VIBuffer::Initialize(void* pArg)
{
	return S_OK;

}

HRESULT VIBuffer::Draw()
{


    return S_OK;
}

