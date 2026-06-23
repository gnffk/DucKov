#include "LaserTrail.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

LaserTrail::LaserTrail(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext): GameObject{ pDevice, pContext }
{
}

LaserTrail::LaserTrail(const LaserTrail& Prototype) : GameObject{ Prototype }
{
}

LaserTrail::~LaserTrail()
{
}

HRESULT LaserTrail::Initialize_Prototype()
{
    return S_OK;
}

HRESULT LaserTrail::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    LASERTRAIL_DESC* pDesc =
        static_cast<LASERTRAIL_DESC*>(pArg);

    if (pDesc != nullptr)
    {
        m_vStartPos = pDesc->vStartPos;
        m_vDir = pDesc->vDir;

        m_fLaserLength = pDesc->fLength;
        m_fLaserWidth = pDesc->fWidth;
        m_fFollowPower = pDesc->fFollowPower;
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_LaserBuffer()))
        return E_FAIL;

    Reset_Laser();

    return S_OK;
}

void LaserTrail::Priority_Update(_float fTimeDelta)
{
    CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONLIGHT, SHARED_THIS(LaserTrail));

}

void LaserTrail::Update(_float fTimeDelta)
{
    if (false == m_bActive)
        return;

    // 외부에서 Update_Laser를 호출하지 않았을 때도 기본값으로 유지
    Update_Laser(m_vStartPos, m_vDir, fTimeDelta);
}

void LaserTrail::Late_Update(_float fTimeDelta)
{
}

HRESULT LaserTrail::Render()
{
    if (false == m_bActive)
        return S_OK;

    return Render_Laser();
}

HRESULT LaserTrail::Ready_Components()
{
    m_pLaserShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype( CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_LaserTrail") ));

    if (m_pLaserShaderCom == nullptr)
        return E_FAIL;

    if (FAILED(__super::Add_Component(TEXT("Com_LaserShader"), m_pLaserShaderCom)))
        return E_FAIL;

    m_pLaserTextureCom =dynamic_pointer_cast<Texture>( CGameInstance::Get().Clone_Prototype( CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Texture_LaserTrail") ));

    if (m_pLaserTextureCom == nullptr)
        return E_FAIL;

    if (FAILED(__super::Add_Component(TEXT("Com_LaserTexture"), m_pLaserTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT LaserTrail::Ready_LaserBuffer()
{
    m_LaserPoints.resize(LASER_POINT_COUNT);
    m_LaserVertices.resize(LASER_VERTEX_COUNT);

    D3D11_BUFFER_DESC BufferDesc{};
    BufferDesc.ByteWidth = sizeof(VTXBULLETTRAIL) * LASER_VERTEX_COUNT;
    BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    BufferDesc.StructureByteStride = sizeof(VTXBULLETTRAIL);

    if (FAILED(m_pDevice->CreateBuffer(&BufferDesc,nullptr,m_pLaserVB.GetAddressOf())))
    {
        return E_FAIL;
    }

    return S_OK;
}

void LaserTrail::Set_Active(_bool bActive)
{
    m_bActive = bActive;

    if (bActive)
        Reset_Laser();
}

void LaserTrail::Reset_Laser()
{
    m_bFirstFrame = true;
    m_fTime = 0.f;

    for (auto& Point : m_LaserPoints)
    {
        Point.vPos = m_vStartPos;
    }
}

void LaserTrail::Update_Laser(const _float3& vStartPos,const _float3& vDir,_float fTimeDelta)
{
    if (false == m_bActive)
        return;

    m_vStartPos = vStartPos;
    m_vDir = vDir;

    m_fTime += fTimeDelta;

    // Point 만들기 
    Build_LaserPoints(vStartPos, vDir, fTimeDelta);

    Build_LaserMesh();
}

void LaserTrail::Build_LaserPoints(const _float3& vStartPos,const _float3& vDir,_float fTimeDelta)
{
    _vector vStart = XMLoadFloat3(&vStartPos);

    _vector vForward = XMLoadFloat3(&vDir);

    if (XMVectorGetX(XMVector3LengthSq(vForward)) <= 0.000001f)
        vForward = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    else
        vForward = XMVector3Normalize(vForward);

    _vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

    _vector vRight = XMVector3Cross(vWorldUp, vForward);

    if (XMVectorGetX(XMVector3LengthSq(vRight)) <= 0.000001f)
        vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
    else
        vRight = XMVector3Normalize(vRight);

    _vector vUp = XMVector3Normalize(XMVector3Cross(vForward, vRight));

    for (uint32_t i = 0; i < LASER_POINT_COUNT; ++i)
    {
        _float fRatio = static_cast<_float>(i) /  static_cast<_float>(LASER_POINT_COUNT - 1);

        _vector vBasePos = vStart + vForward * m_fLaserLength * fRatio;

        _float fWaveWeight = fRatio;

        _float fWaveRight = sinf(m_fTime * 8.f + fRatio * 20.f) * fWaveWeight;

        _float fWaveUp = cosf(m_fTime * 5.f + fRatio * 14.f) *0.35f * fWaveWeight;

        _vector vTargetPos = vBasePos + vRight * fWaveRight + vUp * fWaveUp;

        _vector vCurPos = XMLoadFloat3(&m_LaserPoints[i].vPos);

        _float fFollow =fTimeDelta * m_fFollowPower;

        if (fFollow > 1.f)
            fFollow = 1.f;

        _vector vNewPos =XMVectorLerp(vCurPos, vTargetPos, fFollow);

        if (m_bFirstFrame)
            vNewPos = vTargetPos;

        XMStoreFloat3(&m_LaserPoints[i].vPos,vNewPos);
    }

    m_bFirstFrame = false;
}

void LaserTrail::Build_LaserMesh()
{
    if (m_LaserPoints.size() < 2)
        return;

    _float4  pCamera;
    CGameInstance::Get().Get_MainCameraPosition(pCamera);

    _vector vCamPos = XMVectorSet(pCamera.x, pCamera.y, pCamera.z, pCamera.w);

    for (uint32_t i = 0; i < LASER_POINT_COUNT; ++i)
    {
        _float fRatio = static_cast<_float>(i) / static_cast<_float>(LASER_POINT_COUNT - 1);

        _vector vPos = XMLoadFloat3(&m_LaserPoints[i].vPos);

        _vector vNext{};

        if (i < LASER_POINT_COUNT - 1)
        {
            vNext = XMLoadFloat3(&m_LaserPoints[i + 1].vPos);
        }
        else
        {
            _vector vPrev = XMLoadFloat3(&m_LaserPoints[i - 1].vPos);

            vNext = vPos + (vPos - vPrev);
        }

        _vector vTangent = vNext - vPos;

        if (XMVectorGetX(XMVector3LengthSq(vTangent)) <= 0.000001f)
            vTangent = XMVectorSet(0.f, 0.f, 1.f, 0.f);
        else
            vTangent = XMVector3Normalize(vTangent);

        _vector vToCam = vCamPos - vPos;

        if (XMVectorGetX(XMVector3LengthSq(vToCam)) <= 0.000001f)
            vToCam = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        else
            vToCam = XMVector3Normalize(vToCam);

        _vector vSide = XMVector3Cross(vToCam, vTangent);

        if (XMVectorGetX(XMVector3LengthSq(vSide)) <= 0.000001f)
            vSide = XMVectorSet(1.f, 0.f, 0.f, 0.f);
        else
            vSide = XMVector3Normalize(vSide);

        // 시작/끝은 살짝 얇게
        _float fTip = sinf(fRatio * XM_PI);

        if (fTip < 0.25f)
            fTip = 0.25f;

        _float fWidth = m_fLaserWidth * fTip;

        _vector vLeft = vPos - vSide * fWidth;

        _vector vRight = vPos + vSide * fWidth;

        _float3 vLeftPos{};
        _float3 vRightPos{};

        XMStoreFloat3(&vLeftPos, vLeft);
        XMStoreFloat3(&vRightPos, vRight);

        uint32_t iLeftIndex = i * 2;
        uint32_t iRightIndex = i * 2 + 1;

        m_LaserVertices[iLeftIndex].vPosition = vLeftPos;
        m_LaserVertices[iLeftIndex].vTexcoord = _float2(fRatio * 4.f, 0.f);
        m_LaserVertices[iLeftIndex].vColor = _float4(1.f, 1.f, 1.f, 1.f);

        m_LaserVertices[iRightIndex].vPosition = vRightPos;
        m_LaserVertices[iRightIndex].vTexcoord = _float2(fRatio * 4.f, 1.f);
        m_LaserVertices[iRightIndex].vColor = _float4(1.f, 1.f, 1.f, 1.f);
    }

    D3D11_MAPPED_SUBRESOURCE SubResource{};

    if (SUCCEEDED(m_pContext->Map(m_pLaserVB.Get(),  0,D3D11_MAP_WRITE_DISCARD,0,&SubResource)))
    {
        memcpy(SubResource.pData, m_LaserVertices.data(), sizeof(VTXBULLETTRAIL) * LASER_VERTEX_COUNT);

        m_pContext->Unmap(m_pLaserVB.Get(), 0);
    }
}

HRESULT LaserTrail::Render_Laser()
{
    if (m_pLaserShaderCom == nullptr)
        return E_FAIL;

    if (m_pLaserTextureCom == nullptr)
        return E_FAIL;

    _float4x4 ViewMatrix{};
    _float4x4 ProjMatrix{};

    CGameInstance::Get().Get_MainCameraMatrix( ViewMatrix, ProjMatrix );

    _float4x4 WorldMatrix{};
    XMStoreFloat4x4(&WorldMatrix,XMMatrixIdentity());

    if (FAILED(m_pLaserShaderCom->Bind_Matrix("g_WorldMatrix",&WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pLaserShaderCom->Bind_Matrix("g_ViewMatrix",&ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pLaserShaderCom->Bind_Matrix("g_ProjMatrix",&ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pLaserTextureCom->Bind_ShaderResource(m_pLaserShaderCom,"g_DiffuseTexture",0)))
        return E_FAIL;


    _float fIntensity = 6.f;
    m_pLaserShaderCom->Bind_RawValue("g_fIntensity",&fIntensity,sizeof(_float));

    uint32_t iStride = sizeof(VTXBULLETTRAIL);
    uint32_t iOffset = 0;

    m_pContext->IASetVertexBuffers(0,1,m_pLaserVB.GetAddressOf(),&iStride,&iOffset);

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    if (FAILED(m_pLaserShaderCom->Begin(0)))
        return E_FAIL;

    m_pContext->Draw(LASER_VERTEX_COUNT,0);

    return S_OK;
}

unique_ptr<LaserTrail> LaserTrail::Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance =unique_ptr<LaserTrail>(new LaserTrail(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : LaserTrail");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> LaserTrail::Clone(void* pArg)
{
    auto pInstance =shared_ptr<GameObject>( new LaserTrail(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : LaserTrail");
        return nullptr;
    }

    return pInstance;
}
