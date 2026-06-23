#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)

class Shader;
class Texture;

NS_END

NS_BEGIN(Client)

class LaserTrail final : public GameObject
{
public:
    typedef struct tagLaserTrailDesc : public GameObject::GAMEOBJECT_DESC
    {
        _float3 vStartPos = {};
        _float3 vDir = { 0.f, 0.f, 1.f };

        _float fLength = 35.f;
        _float fWidth = 0.7f;

        _float fFollowPower = 10.f;
        _float fWavePower = 0.8f;

    } LASERTRAIL_DESC;

private:
    struct LASER_POINT
    {
        _float3 vPos = {};
    };

private:
    LaserTrail(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext);

    LaserTrail(const LaserTrail& Prototype);

public:
    virtual ~LaserTrail();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;

    virtual HRESULT Render() override;

public:
    // 보스 패턴에서 매 프레임 호출할 함수
    void Update_Laser(const _float3& vStartPos, const _float3& vDir,_float fTimeDelta);

    void Set_Active(_bool bActive);
    _bool Is_Active() const { return m_bActive; }

    void Reset_Laser();

private:
    HRESULT Ready_Components();
    HRESULT Ready_LaserBuffer();

private:
    void Build_LaserPoints(const _float3& vStartPos,const _float3& vDir,_float fTimeDelta);

    void Build_LaserMesh();
    HRESULT Render_Laser();

private:
    vector<LASER_POINT> m_LaserPoints;
    vector<VTXBULLETTRAIL> m_LaserVertices;

private:
    ComPtr<ID3D11Buffer> m_pLaserVB = nullptr;

private:
    shared_ptr<Shader>  m_pLaserShaderCom = nullptr;
    shared_ptr<Texture> m_pLaserTextureCom = nullptr;

private:
    _bool m_bActive = false;
    _bool m_bFirstFrame = true;

private:
    _float m_fTime = 0.f;

    _float3 m_vStartPos = {};
    _float3 m_vDir = { 0.f, 0.f, 1.f };

private:
    _float m_fLaserLength = 35.f;
    _float m_fLaserWidth = 0.7f;

    // 레이저가 바로 직선으로 안 따라가고 늦게 따라오게 하는 값
    _float m_fFollowPower = 10.f;


private:
    static constexpr uint32_t LASER_POINT_COUNT = 10;
    static constexpr uint32_t LASER_VERTEX_COUNT = LASER_POINT_COUNT * 2;

public:
    static unique_ptr<LaserTrail> Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext);

    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END