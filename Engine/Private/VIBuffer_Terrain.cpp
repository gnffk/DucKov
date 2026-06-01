#include "VIBuffer_Terrain.h"

VIBuffer_Terrain::VIBuffer_Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : VIBuffer{ pDevice, pContext }
{
}

VIBuffer_Terrain::~VIBuffer_Terrain()
{

}


HRESULT VIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
    wstring path = pHeightMapFilePath;

    if (path.ends_with(L".raw"))
    {
        DWORD dwByte = {};

        HANDLE hFile = CreateFile(
            pHeightMapFilePath,
            GENERIC_READ,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0);

        if (INVALID_HANDLE_VALUE == hFile)
            return E_FAIL;

        // Unity Terrain RAW 해상도
        // Unity에서 Export한 Heightmap Resolution 값으로 맞춰야 함
        const uint32_t iRawWidth = 513;
        const uint32_t iRawHeight = 513;

        m_iNumVertexBuffers = 1;
        m_iNumVerticesX = iRawWidth;
        m_iNumVerticesZ = iRawHeight;
        m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
        m_iVertexStride = sizeof(VTXNORTEX);
        m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
        m_iIndexStride = 4;
        m_eIndexFormat = DXGI_FORMAT_R32_UINT;
        m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        unique_ptr<uint16_t[]> pHeights =make_unique<uint16_t[]>(m_iNumVertices);

        ReadFile(
            hFile,
            pHeights.get(),
            sizeof(uint16_t) * m_iNumVertices,
            &dwByte,
            nullptr);

        CloseHandle(hFile);

#pragma region VERTEX_BUFFER

        D3D11_BUFFER_DESC VertexBufferDesc{};
        VertexBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
        VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        VertexBufferDesc.StructureByteStride = m_iVertexStride;

        m_vertices = make_shared<vector<VTXNORTEX>>();
        m_vertices->resize(m_iNumVertices);
     
        const float fHeightScale = 128.f;

        for (uint32_t i = 0; i < m_iNumVerticesZ; ++i)
        {
            for (uint32_t j = 0; j < m_iNumVerticesX; ++j)
            {
                uint32_t iIndex = i * m_iNumVerticesX + j;

                float fHeight =
                    pHeights[iIndex] / 65535.f;

                fHeight *= fHeightScale;

                (*m_vertices)[iIndex].vPosition = _float3((float)j, fHeight, (float)i);

                (*m_vertices)[iIndex].vNormal = _float3(0.f, 0.f, 0.f);

                (*m_vertices)[iIndex].vTexcoord = _float2(
                 (float)j / (float)(m_iNumVerticesX - 1),
                        (float)i / (float)(m_iNumVerticesZ - 1));
            }
        }

#pragma endregion

#pragma region INDEX_BUFFER

        D3D11_BUFFER_DESC IndexBufferDesc{};
        IndexBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
        IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        IndexBufferDesc.StructureByteStride = m_iIndexStride;

        m_indices = make_shared<vector<uint32_t>>();
        m_indices->resize(m_iNumIndices);

        uint32_t iNumIndices = 0;

        for (uint32_t i = 0; i < m_iNumVerticesZ - 1; ++i)
        {
            for (uint32_t j = 0; j < m_iNumVerticesX - 1; ++j)
            {
                uint32_t iIndex = i * m_iNumVerticesX + j;

                uint32_t iIndices[4] =
                {
                    iIndex + m_iNumVerticesX,
                    iIndex + m_iNumVerticesX + 1,
                    iIndex + 1,
                    iIndex
                };

                _vector vSour, vDest, vNormal;

                (*m_indices)[iNumIndices++] = iIndices[0];
                (*m_indices)[iNumIndices++] = iIndices[1];
                (*m_indices)[iNumIndices++] = iIndices[2];

                vSour = XMLoadFloat3(&(*m_vertices)[iIndices[1]].vPosition) - XMLoadFloat3(&(*m_vertices)[iIndices[0]].vPosition);
                vDest = XMLoadFloat3(&(*m_vertices)[iIndices[2]].vPosition) - XMLoadFloat3(&(*m_vertices)[iIndices[1]].vPosition);
                vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

                XMStoreFloat3(&(*m_vertices)[iIndices[0]].vNormal,XMLoadFloat3(&(*m_vertices)[iIndices[0]].vNormal) + vNormal);
                XMStoreFloat3(&(*m_vertices)[iIndices[1]].vNormal,XMLoadFloat3(&(*m_vertices)[iIndices[1]].vNormal) + vNormal);
                XMStoreFloat3(&(*m_vertices)[iIndices[2]].vNormal,XMLoadFloat3(&(*m_vertices)[iIndices[2]].vNormal) + vNormal);

                (*m_indices)[iNumIndices++] = iIndices[0];
                (*m_indices)[iNumIndices++] = iIndices[2];
                (*m_indices)[iNumIndices++] = iIndices[3];

                vSour = XMLoadFloat3(&(*m_vertices)[iIndices[2]].vPosition) - XMLoadFloat3(&(*m_vertices)[iIndices[0]].vPosition);
                vDest = XMLoadFloat3(&(*m_vertices)[iIndices[3]].vPosition) - XMLoadFloat3(&(*m_vertices)[iIndices[2]].vPosition);
                vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

                XMStoreFloat3(&(*m_vertices)[iIndices[0]].vNormal, XMLoadFloat3(&(*m_vertices)[iIndices[0]].vNormal) + vNormal);
                XMStoreFloat3(&(*m_vertices)[iIndices[2]].vNormal, XMLoadFloat3(&(*m_vertices)[iIndices[2]].vNormal) + vNormal);
                XMStoreFloat3(&(*m_vertices)[iIndices[3]].vNormal, XMLoadFloat3(&(*m_vertices)[iIndices[3]].vNormal) + vNormal);
            }
        }

        for (size_t i = 0; i < m_iNumVertices; ++i)
        {
            XMStoreFloat3(&(*m_vertices)[i].vNormal,XMVector3Normalize(XMLoadFloat3(&(*m_vertices)[i].vNormal)));
        }

        D3D11_SUBRESOURCE_DATA VertexInitialData{};
        VertexInitialData.pSysMem = m_vertices->data();

        if (FAILED(m_pDevice->CreateBuffer(
            &VertexBufferDesc,
            &VertexInitialData,
            &m_pVB)))
            return E_FAIL;

        D3D11_SUBRESOURCE_DATA IndexInitialData{};
        IndexInitialData.pSysMem = m_indices->data();

        if (FAILED(m_pDevice->CreateBuffer(
            &IndexBufferDesc,
            &IndexInitialData,
            &m_pIB)))
            return E_FAIL;

#pragma endregion
    }
    else
    {
        DWORD       dwByte = {};
        HANDLE      hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (0 == hFile)
            return E_FAIL;

        BITMAPFILEHEADER            fh{};
        BITMAPINFOHEADER            ih{};

        ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
        ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

        unique_ptr<uint32_t[]>      pPixels = unique_ptr<uint32_t[]>(new uint32_t[ih.biWidth * ih.biHeight]);
        ReadFile(hFile, pPixels.get(), sizeof(uint32_t) * ih.biWidth * ih.biHeight, &dwByte, nullptr);


        m_iNumVertexBuffers = 1;
        m_iNumVerticesX = ih.biWidth;
        m_iNumVerticesZ = ih.biHeight;
        m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
        m_iVertexStride = sizeof(VTXNORTEX);
        m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
        m_iIndexStride = 4;
        m_eIndexFormat = DXGI_FORMAT_R32_UINT;
        m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

        D3D11_BUFFER_DESC           VertexBufferDesc{};
        VertexBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
        VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        VertexBufferDesc.StructureByteStride = m_iVertexStride;
        VertexBufferDesc.CPUAccessFlags = 0;
        VertexBufferDesc.MiscFlags = 0;

        unique_ptr<VTXNORTEX[]> pVertices = unique_ptr<VTXNORTEX[]>(new VTXNORTEX[m_iNumVertices]);
        ZeroMemory(pVertices.get(), sizeof(VTXNORTEX) * m_iNumVertices);

        for (uint32_t i = 0; i < m_iNumVerticesZ; i++)
        {
            for (uint32_t j = 0; j < m_iNumVerticesX; j++)
            {
                uint32_t        iIndex = i * m_iNumVerticesX + j;


                pVertices[iIndex].vPosition = _float3(j, (pPixels[iIndex] & 0x000000ff) / 10.f, i);
                pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
                pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
            }
        }




#pragma endregion


#pragma region INDEX_BUFFER
        D3D11_BUFFER_DESC           IndexBufferDesc{};
        IndexBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
        IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        IndexBufferDesc.StructureByteStride = m_iIndexStride;
        IndexBufferDesc.CPUAccessFlags = 0;
        IndexBufferDesc.MiscFlags = 0;

        unique_ptr<uint32_t[]> pIndices = unique_ptr<uint32_t[]>(new uint32_t[m_iNumIndices]);
        ZeroMemory(pIndices.get(), sizeof(uint32_t) * m_iNumIndices);

        uint32_t        iNumIndices = {};

        for (uint32_t i = 0; i < m_iNumVerticesZ - 1; i++)
        {
            for (uint32_t j = 0; j < m_iNumVerticesX - 1; j++)
            {
                uint32_t        iIndex = i * m_iNumVerticesX + j;

                uint32_t        iIndices[4] = {
                    iIndex + m_iNumVerticesX,
                    iIndex + m_iNumVerticesX + 1,
                    iIndex + 1,
                    iIndex
                };

                _vector     vSour, vDest, vNormal;

                pIndices[iNumIndices++] = iIndices[0];
                pIndices[iNumIndices++] = iIndices[1];
                pIndices[iNumIndices++] = iIndices[2];

                vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
                vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
                vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

                XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
                XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
                XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);


                pIndices[iNumIndices++] = iIndices[0];
                pIndices[iNumIndices++] = iIndices[2];
                pIndices[iNumIndices++] = iIndices[3];


                vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
                vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
                vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

                XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
                XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
                XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
            }
        }



        //hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation_Terrain.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        //if (0 == hFile)
        //    return E_FAIL;

        //for (size_t i = 0; i < m_iNumIndices; ++i)
        //    WriteFile(hFile, &pVertices[pIndices[i]].vPosition, sizeof(_float3), &dwByte, nullptr);




        for (size_t i = 0; i < m_iNumVertices; i++)
            XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));


        D3D11_SUBRESOURCE_DATA          VertexInitialData{};
        VertexInitialData.pSysMem = pVertices.get();

        if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
            return E_FAIL;

        D3D11_SUBRESOURCE_DATA          IndexInitialData{};
        IndexInitialData.pSysMem = pIndices.get();

        if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
            return E_FAIL;

#pragma endregion

        CloseHandle(hFile);
    }


    return S_OK;
}

HRESULT VIBuffer_Terrain::Initialize(void* pArg)
{

    return S_OK;
}

unique_ptr<VIBuffer_Terrain> VIBuffer_Terrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pHeightMapFilePath)
{
    auto		pInstance = unique_ptr<VIBuffer_Terrain>(new VIBuffer_Terrain(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
    {
        MSG_BOX("Failed to Created : VIBuffer_Terrain");
        return nullptr;
    }

    return pInstance;
}



shared_ptr<Prototype> VIBuffer_Terrain::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<VIBuffer_Terrain>(new VIBuffer_Terrain(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : VIBuffer_Terrain");
        return nullptr;
    }

    return pInstance;
}

