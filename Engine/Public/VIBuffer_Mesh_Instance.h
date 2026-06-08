#pragma once
#include "Component.h"
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Mesh_Instance final : public VIBuffer_Instance
{
public:
    VIBuffer_Mesh_Instance(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

    VIBuffer_Mesh_Instance(const VIBuffer_Mesh_Instance& Prototype);

    virtual ~VIBuffer_Mesh_Instance();

public:
    HRESULT Initialize(shared_ptr<vector<VTXMESH>> pvertices,shared_ptr<vector<uint32_t>> pindices,uint32_t materialIndex, _fmatrix PreTransformMatrix);

    HRESULT Create_InstanceBuffer(uint32_t iCapacity);
    HRESULT Update_InstanceBuffer(const vector<_float4x4>& InstanceWorlds);

    HRESULT Bind_Resources();
    HRESULT Render();

    uint32_t Get_MaterialIndex() const {
        return m_iMaterialIndex;
    }

public:
    static shared_ptr<VIBuffer_Mesh_Instance> Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext,shared_ptr<vector<VTXMESH>> pvertices,shared_ptr<vector<uint32_t>> pindices,uint32_t materialIndex,_fmatrix PreTransformMatrix);

    virtual shared_ptr<Prototype> Clone(void* pArg) override;

private:
    shared_ptr<vector<VTXMESH>>     vertices;
    shared_ptr<vector<uint32_t>>    indices;

    uint32_t                        m_iMaterialIndex = 0;

    ComPtr<ID3D11Buffer>            m_pInstanceVB = nullptr;

    uint32_t                        m_iInstanceStride = sizeof(_float4x4);
    uint32_t                        m_iNumInstances = 0;
    uint32_t                        m_iInstanceCapacity = 0;
};
NS_END
