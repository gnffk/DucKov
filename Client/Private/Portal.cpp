#include "Portal.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Transform.h"

#include "AABB_Collider.h"
#include "OBB_Collider.h"
#include "Sphere_Collider.h"
#include "BaseCollider.h"

#include "UIObject.h"
#include "PortalUI.h"

Portal::Portal(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : GameObject{ pDevice, pContext }
{
}

Portal::Portal(const Portal& Prototype)
    : GameObject{ Prototype }
{
}

Portal::~Portal()
{
}

HRESULT Portal::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Portal::Initialize(void* pArg)
{
    PORTAL_DESC* pDesc = static_cast<PORTAL_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (pDesc != nullptr)
    {
        m_eNextLevel = pDesc->eNextLevel;

        if (m_pTransformCom != nullptr)
        {
            _vector vPos = XMLoadFloat3(&pDesc->vSpawnPos);
            vPos = XMVectorSetW(vPos, 1.f);

            m_pTransformCom->Set_State(STATE::POSITION, vPos);
        }
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_UI()))
        return E_FAIL;

    // 포탈 모델 크기
    m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);

    return S_OK;
}

void Portal::Priority_Update(_float fTimeDelta)
{
    CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND,SHARED_THIS(Portal));

    for (int type = 0; type < (int)COLLIDER::COLLIDER_END; ++type)
    {
        auto& colliderList = m_pColliderComs[type];

        for (size_t i = 0; i < colliderList.size(); ++i)
        {
            CGameInstance::Get().Add_Collider(colliderList[i]->Get_GroupTag(),colliderList[i].get());
        }
    }
}

void Portal::Update(_float fTimeDelta)
{
}

void Portal::Late_Update(_float fTimeDelta)
{

    Collider_Player(fTimeDelta);

    if (GetCollision())
    {
        for (auto& Pair : m_pUI)
        {
            const string& strUIName = Pair.first;
            auto pUIObject = Pair.second;

            if (nullptr == pUIObject)
                continue;

            auto pPortalUI = static_pointer_cast<PortalUI>(pUIObject);

            if (pPortalUI == nullptr)
                continue;

            // Owner가 없으면 Portal을 Owner로 세팅
            if (pPortalUI->GetOwner().lock() == nullptr)
            {
                pPortalUI->Set_Owner(SHARED_THIS(Portal));
            }

            CGameInstance::Get().Add_UIObject( L"PortalUI",static_pointer_cast<UIObject>(pUIObject));
        }
    }

}


HRESULT Portal::Render()
{
    _float4x4 View, Proj;
    CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

    _float4x4 World = m_pTransformCom->GetWorldMatrix();

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
        return E_FAIL;

    uint32_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (uint32_t i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Materials(
            m_pShaderCom,
            "g_DiffuseTexture",
            i,
            ETOUI(TEXTURETYPE::DIFFUSE),
            0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(
            m_pShaderCom,
            "g_BoneMatrices",
            i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

_bool Portal::Collider_Player(_float fTimeDelta)
{
    auto Collider = m_pColliderComs[(int)COLLIDER::COLLIDER_OBB].front().get();
    auto ColliderPlayer = CGameInstance::Get().GetColliderGroups(L"Player");

    _bool bHitInteractBox = false;


    for (auto _PlayerCollider : *ColliderPlayer)
    {
        if (CGameInstance::Get().Intersect(_PlayerCollider, Collider))
        {
            bHitInteractBox = true;
            m_bCollision = bHitInteractBox;
            Collider->SetColliderColor(ColliderColor::RED);
            _PlayerCollider->SetColliderColor(ColliderColor::RED);
        }
    }
    

    return _bool();
}

HRESULT Portal::Ready_Components()
{
    __super::Clear_Compnent();

    // 예: Object_INFO.m_strPrototypeBaseName == L"SM_Portal"
    m_ModelComponentName =
        TEXT("Prototype_Com_Model_") + Object_INFO.m_strPrototypeBaseName;

    m_pModelCom = dynamic_pointer_cast<Model>(
        CGameInstance::Get().Clone_Prototype(
            CGameInstance::Get().Get_Level(),
            m_ModelComponentName));

    if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
        return E_FAIL;

    m_pShaderCom = dynamic_pointer_cast<Shader>(
        CGameInstance::Get().Clone_Prototype(
            CGameInstance::Get().Get_Level(),
            TEXT("Prototype_Com_Shader_Vtx_FBX_Tex")));

    if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
        return E_FAIL;

    auto pCollider = dynamic_pointer_cast<BaseCollider>(
        CGameInstance::Get().Clone_Prototype(
            CGameInstance::Get().Get_Level(),
            TEXT("Prototype_Com_OBB_Collider")));

    if (pCollider == nullptr)
        return E_FAIL;

    pCollider->SetOwner(SHARED_THIS(Portal).get());

    if (FAILED(__super::Add_Component(TEXT("Com_OBBCollider"), pCollider)))
        return E_FAIL;

    // Player 쪽에서 InteractBox랑 충돌 검사 중이면 같은 그룹 써도 됨.
    // 기존 Player::Collider_Box가 L"InteractBox"만 보고 있다면 반드시 이걸로.
    pCollider->Set_GroupTag(L"InteractBox");

    pCollider->Set_Center(_float3{ 0.f, 1.0f, 0.f });
    pCollider->Set_Extend(_float3{ 0.8f, 10.f, 0.8f });

    m_pColliderComs[(int)COLLIDER::COLLIDER_OBB].push_back(pCollider);

    return S_OK;
}

HRESULT Portal::Ready_UI()
{
    UIObject::UIOBJECT_DESC DescUI{};

    DescUI.ObjectType = ETOUI(OBJECTTYPE::OBJECT_UI);
    DescUI.m_strName = L"PortalUI";
    DescUI.m_strPrototypeObjectName = L"Prototype_GameObject_PortalUI";
    DescUI.m_strPrototypeBaseName = L"PortalUI";
    DescUI.pCameraType = ETOUI(CAMERA::NONE);

    DescUI.fSpeedPerSec = 5.f;
    DescUI.fRotationPerSec = 1.f;
    DescUI.fSizeX = 1.f;
    DescUI.fSizeY = 1.f;
    DescUI.fX = 1.f;
    DescUI.fY = 1.f;

    auto pPortalUIObject =dynamic_pointer_cast<GameObject>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_GameObject_PortalUI"),&DescUI));

    if (pPortalUIObject == nullptr)
        return E_FAIL;

    dynamic_pointer_cast<PortalUI>(pPortalUIObject)->Set_Owner(SHARED_THIS(Portal));

    m_pUI.emplace("PortalUI", pPortalUIObject);

    return S_OK;
}

unique_ptr<Portal> Portal::Create(
    ComPtr<ID3D11Device> pDevice,
    ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance =
        unique_ptr<Portal>(new Portal(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Portal");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> Portal::Clone(void* pArg)
{
    auto pInstance =
        shared_ptr<GameObject>(new Portal(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Portal");
        return nullptr;
    }

    return pInstance;
}