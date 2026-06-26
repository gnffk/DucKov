#include "Lightning.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "Particle_Spark.h"

NS_BEGIN(Client)

Lightning::Lightning(
    ComPtr<ID3D11Device> pDevice,
    ComPtr<ID3D11DeviceContext> pContext)
    : GameObject{ pDevice, pContext }
{
}

Lightning::Lightning(const Lightning& Prototype)
    : GameObject{ Prototype }
{
}

Lightning::~Lightning()
{
}

HRESULT Lightning::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Lightning::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    LIGHTNING_DESC* pDesc = static_cast<LIGHTNING_DESC*>(pArg);

    if (pDesc != nullptr)
    {
        m_vStartPos = pDesc->vStartPos;
        m_vDir = pDesc->vDir;

        m_fLength = pDesc->fLength;
        m_fWidth = pDesc->fWidth;

        m_fLifeTime = pDesc->fLifeTime;
        m_fJitterPower = pDesc->fJitterPower;

        m_fRefreshInterval = pDesc->fRefreshInterval;
        m_fSparkInterval = pDesc->fSparkInterval;

        m_bSpawnSpark = pDesc->bSpawnSpark;
        m_bAutoDead = pDesc->bAutoDead;

        m_bStartActive = pDesc->bStartActive;
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_LightningBuffer()))
        return E_FAIL;

    Reset_Lightning();

    if (m_bStartActive)
        Set_Active(true);

    return S_OK;

}

void Lightning::Priority_Update(_float fTimeDelta)
{
    if (m_bActive)
    {
        CGameInstance::Get().Add_RenderObject(
            RENDERGROUP::NONLIGHT,
            SHARED_THIS(Lightning)
        );
    }
}

void Lightning::Update(_float fTimeDelta)
{
    if (false == m_bActive)
        return;

    m_fLifeTimer += fTimeDelta;

    if (m_bAutoDead && m_fLifeTimer >= m_fLifeTime)
    {
        m_bActive = false;
        Set_Dead();
        return;
    }

    Update_Lightning(m_vStartPos, m_vDir, fTimeDelta);
}

void Lightning::Late_Update(_float fTimeDelta)
{
}

HRESULT Lightning::Render()
{
    if (false == m_bActive)
        return S_OK;

    return Render_Lightning();
}

HRESULT Lightning::Ready_Components()
{
    m_pLightningShaderCom =
        dynamic_pointer_cast<Shader>(
            CGameInstance::Get().Clone_Prototype(
                CGameInstance::Get().Get_Level(),
                TEXT("Prototype_Com_Shader_LaserTrail")
            )
        );

    if (m_pLightningShaderCom == nullptr)
        return E_FAIL;

    if (FAILED(__super::Add_Component(
        TEXT("Com_LightningShader"),
        m_pLightningShaderCom)))
        return E_FAIL;

    // 일단 LaserTrail 텍스처 재사용.
    // 나중에 번개 전용 texture 만들면 Prototype_Com_Texture_LightningTrail로 교체.
    m_pLightningTextureCom =
        dynamic_pointer_cast<Texture>(
            CGameInstance::Get().Clone_Prototype(
                CGameInstance::Get().Get_Level(),
                TEXT("Prototype_Com_Texture_LightningTrail")
            )
        );

    if (m_pLightningTextureCom == nullptr)
        return E_FAIL;

    if (FAILED(__super::Add_Component(
        TEXT("Com_LightningTexture"),
        m_pLightningTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT Lightning::Ready_LightningBuffer()
{
    m_LightningPoints.resize(LIGHTNING_POINT_COUNT);
    m_LightningVertices.resize(LIGHTNING_VERTEX_COUNT);

    D3D11_BUFFER_DESC BufferDesc{};
    BufferDesc.ByteWidth =
        sizeof(VTXBULLETTRAIL) * LIGHTNING_VERTEX_COUNT;

    BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    BufferDesc.StructureByteStride = sizeof(VTXBULLETTRAIL);

    if (FAILED(m_pDevice->CreateBuffer(
        &BufferDesc,
        nullptr,
        m_pLightningVB.GetAddressOf())))
    {
        return E_FAIL;
    }

    return S_OK;
}

void Lightning::Set_Active(_bool bActive)
{
    m_bActive = bActive;

    if (bActive)
        Reset_Lightning();
}

void Lightning::Reset_Lightning()
{
    m_bFirstFrame = true;
    m_fLifeTimer = 0.f;
    m_fRefreshTimer = 0.f;
    m_fSparkTimer = 0.f;

    for (auto& Point : m_LightningPoints)
        Point.vPos = m_vStartPos;
}

void Lightning::Update_Lightning(
    const _float3& vStartPos,
    const _float3& vDir,
    _float fTimeDelta)
{
    if (false == m_bActive)
        return;

    m_vStartPos = vStartPos;
    m_vDir = vDir;

    m_fRefreshTimer -= fTimeDelta;
    m_fSparkTimer -= fTimeDelta;

    // 번개는 매 프레임 부드럽게 움직이는 것보다
    // 짧은 주기로 모양이 팍팍 바뀌는 게 더 번개 같음.
    if (m_fRefreshTimer <= 0.f || m_bFirstFrame)
    {
        m_fRefreshTimer = m_fRefreshInterval;

        Build_LightningPoints(vStartPos, vDir);
        Build_LightningMesh();

        m_bFirstFrame = false;
    }

    if (m_bSpawnSpark && m_fSparkTimer <= 0.f)
    {
        m_fSparkTimer = m_fSparkInterval;

        // 끝점에 Spark
        _float3 vEndPos = m_LightningPoints.back().vPos;
        Spawn_SparkAt(vEndPos);

        // 중간에도 가끔 Spark
        uint32_t iMid = LIGHTNING_POINT_COUNT / 2;
        Spawn_SparkAt(m_LightningPoints[iMid].vPos);
    }
}

void Lightning::Build_LightningPoints(
    const _float3& vStartPos,
    const _float3& vDir)
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

    for (uint32_t i = 0; i < LIGHTNING_POINT_COUNT; ++i)
    {
        _float fRatio =
            static_cast<_float>(i) /
            static_cast<_float>(LIGHTNING_POINT_COUNT - 1);

        _vector vBasePos =
            vStart + vForward * m_fLength * fRatio;

        // 시작/끝은 흔들림 약하게, 중간은 강하게
        _float fCenterWeight = sinf(fRatio * XM_PI);

        _float fRandRight =
            Random_Range(-1.f, 1.f) *
            m_fJitterPower *
            fCenterWeight;

        _float fRandUp =
            Random_Range(-1.f, 1.f) *
            m_fJitterPower *
            0.6f *
            fCenterWeight;

        _vector vFinalPos =
            vBasePos +
            vRight * fRandRight +
            vUp * fRandUp;

        XMStoreFloat3(&m_LightningPoints[i].vPos, vFinalPos);
    }
}

void Lightning::Build_LightningMesh()
{
    if (m_LightningPoints.size() < 2)
        return;

    _float4 vCameraPos{};
    CGameInstance::Get().Get_MainCameraPosition(vCameraPos);

    _vector vCamPos =
        XMVectorSet(vCameraPos.x, vCameraPos.y, vCameraPos.z, vCameraPos.w);

    for (uint32_t i = 0; i < LIGHTNING_POINT_COUNT; ++i)
    {
        _float fRatio =
            static_cast<_float>(i) /
            static_cast<_float>(LIGHTNING_POINT_COUNT - 1);

        _vector vPos = XMLoadFloat3(&m_LightningPoints[i].vPos);

        _vector vNext{};

        if (i < LIGHTNING_POINT_COUNT - 1)
        {
            vNext = XMLoadFloat3(&m_LightningPoints[i + 1].vPos);
        }
        else
        {
            _vector vPrev = XMLoadFloat3(&m_LightningPoints[i - 1].vPos);
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

        _float fTip = sinf(fRatio * XM_PI);

        if (fTip < 0.18f)
            fTip = 0.18f;

        // 번개는 레이저보다 얇게
        _float fWidth = m_fWidth * fTip;

        _vector vLeft = vPos - vSide * fWidth;
        _vector vRight = vPos + vSide * fWidth;

        _float3 vLeftPos{};
        _float3 vRightPos{};

        XMStoreFloat3(&vLeftPos, vLeft);
        XMStoreFloat3(&vRightPos, vRight);

        uint32_t iLeftIndex = i * 2;
        uint32_t iRightIndex = i * 2 + 1;

        // 끝으로 갈수록 alpha 약간 감소
        _float fAlpha = 1.f - fRatio * 0.25f;

        m_LightningVertices[iLeftIndex].vPosition = vLeftPos;
        m_LightningVertices[iLeftIndex].vTexcoord = _float2(fRatio * 6.f, 0.f);
        m_LightningVertices[iLeftIndex].vColor = _float4(0.4f, 0.8f, 1.f, fAlpha);

        m_LightningVertices[iRightIndex].vPosition = vRightPos;
        m_LightningVertices[iRightIndex].vTexcoord = _float2(fRatio * 6.f, 1.f);
        m_LightningVertices[iRightIndex].vColor = _float4(1.f, 1.f, 1.f, fAlpha);
    }

    D3D11_MAPPED_SUBRESOURCE SubResource{};

    if (SUCCEEDED(m_pContext->Map(
        m_pLightningVB.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &SubResource)))
    {
        memcpy(
            SubResource.pData,
            m_LightningVertices.data(),
            sizeof(VTXBULLETTRAIL) * LIGHTNING_VERTEX_COUNT
        );

        m_pContext->Unmap(m_pLightningVB.Get(), 0);
    }
}

HRESULT Lightning::Render_Lightning()
{
    if (m_pLightningShaderCom == nullptr)
        return E_FAIL;

    if (m_pLightningTextureCom == nullptr)
        return E_FAIL;

    _float4x4 ViewMatrix{};
    _float4x4 ProjMatrix{};

    CGameInstance::Get().Get_MainCameraMatrix(ViewMatrix, ProjMatrix);

    _float4x4 WorldMatrix{};
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

    if (FAILED(m_pLightningShaderCom->Bind_Matrix(
        "g_WorldMatrix",
        &WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pLightningShaderCom->Bind_Matrix(
        "g_ViewMatrix",
        &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pLightningShaderCom->Bind_Matrix(
        "g_ProjMatrix",
        &ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pLightningTextureCom->Bind_ShaderResource(
        m_pLightningShaderCom,
        "g_DiffuseTexture",
        0)))
        return E_FAIL;

    _float fIntensity = 20.f;

    m_pLightningShaderCom->Bind_RawValue(
        "g_fIntensity",
        &fIntensity,
        sizeof(_float)
    );

    uint32_t iStride = sizeof(VTXBULLETTRAIL);
    uint32_t iOffset = 0;

    m_pContext->IASetVertexBuffers(
        0,
        1,
        m_pLightningVB.GetAddressOf(),
        &iStride,
        &iOffset
    );

    m_pContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
    );

    if (FAILED(m_pLightningShaderCom->Begin(0)))
        return E_FAIL;

    m_pContext->Draw(LIGHTNING_VERTEX_COUNT, 0);

    return S_OK;
}

void Lightning::Spawn_SparkAt(const _float3& vPos)
{


    Particle_System::PARTICLE_SPAWN_DESC Desc{};
    Desc.vSpawnPos = vPos;
    Desc.iCount = 30;
    Desc.fPower = 1.5f;

    CGameInstance::Get().Add_Particle(PARTICLE_TYPE::SPARK, &Desc);
}

_float Lightning::Random_Range(_float fMin, _float fMax)
{
    _float fRatio =
        static_cast<_float>(rand()) /
        static_cast<_float>(RAND_MAX);

    return fMin + (fMax - fMin) * fRatio;
}

unique_ptr<Lightning> Lightning::Create(
    ComPtr<ID3D11Device> pDevice,
    ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance =
        unique_ptr<Lightning>(new Lightning(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Lightning");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> Lightning::Clone(void* pArg)
{
    auto pInstance =
        shared_ptr<GameObject>(new Lightning(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Lightning");
        return nullptr;
    }

    return pInstance;
}

NS_END