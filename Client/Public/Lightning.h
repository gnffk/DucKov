#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
NS_END

NS_BEGIN(Client)

class Lightning final : public GameObject
{
public:
    static constexpr uint32_t LIGHTNING_POINT_COUNT = 18;
    static constexpr uint32_t LIGHTNING_VERTEX_COUNT = LIGHTNING_POINT_COUNT * 2;

public:
    typedef struct tagLightningDesc : public GameObject::GAMEOBJECT_DESC
    {
        _float3 vStartPos = { 0.f, 0.f, 0.f };
        _float3 vDir = { 0.f, 0.f, 1.f };

        _float fLength = 12.f;
        _float fWidth = 0.12f;

        _float fLifeTime = 0.25f;
        _float fJitterPower = 0.8f;

        // 번개 모양이 바뀌는 주기
        _float fRefreshInterval = 0.035f;

        // Spark 생성 주기
        _float fSparkInterval = 0.04f;

        _bool bSpawnSpark = true;
        _bool bAutoDead = true;

        _bool bStartActive = true;

    } LIGHTNING_DESC;

private:
    Lightning(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    Lightning(const Lightning& Prototype);

public:
    virtual ~Lightning();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Set_Active(_bool bActive);
    void Reset_Lightning();

    void Update_Lightning(
        const _float3& vStartPos,
        const _float3& vDir,
        _float fTimeDelta
    );

    void Set_Length(_float fLength) { m_fLength = fLength; }
    void Set_Width(_float fWidth) { m_fWidth = fWidth; }
    void Set_JitterPower(_float fPower) { m_fJitterPower = fPower; }

private:
    HRESULT Ready_Components();
    HRESULT Ready_LightningBuffer();

private:
    void Build_LightningPoints(
        const _float3& vStartPos,
        const _float3& vDir
    );

    void Build_LightningMesh();
    HRESULT Render_Lightning();

private:
    void Spawn_SparkAt(const _float3& vPos);
    _float Random_Range(_float fMin, _float fMax);

private:
    struct LIGHTNING_POINT
    {
        _float3 vPos = { 0.f, 0.f, 0.f };
    };

private:
    _bool m_bActive = false;
    _bool m_bFirstFrame = true;
    _bool m_bSpawnSpark = true;
    _bool m_bAutoDead = true;
private:
    _bool m_bStartActive = true;
private:
    _float3 m_vStartPos = { 0.f, 0.f, 0.f };
    _float3 m_vDir = { 0.f, 0.f, 1.f };

    _float m_fLength = 12.f;
    _float m_fWidth = 0.12f;

    _float m_fLifeTime = 0.25f;
    _float m_fLifeTimer = 0.f;

    _float m_fJitterPower = 0.8f;

    _float m_fRefreshInterval = 0.035f;
    _float m_fRefreshTimer = 0.f;

    _float m_fSparkInterval = 0.04f;
    _float m_fSparkTimer = 0.f;

private:
    vector<LIGHTNING_POINT> m_LightningPoints;
    vector<VTXBULLETTRAIL> m_LightningVertices;

private:
    ComPtr<ID3D11Buffer> m_pLightningVB = nullptr;

    shared_ptr<Shader> m_pLightningShaderCom = nullptr;
    shared_ptr<Texture> m_pLightningTextureCom = nullptr;

public:
    static unique_ptr<Lightning> Create(
        ComPtr<ID3D11Device> pDevice,
        ComPtr<ID3D11DeviceContext> pContext
    );

    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END