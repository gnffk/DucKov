#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class Model;
class Shader;
class BaseCollider;
NS_END

NS_BEGIN(Client)

class Portal final : public GameObject
{
public:
    typedef struct tagPortalDesc : public GameObject::GAMEOBJECT_DESC
    {
        _float3 vSpawnPos = {};
        LEVEL eNextLevel = LEVEL::GAMEPLAY;
    } PORTAL_DESC;

private:
    Portal(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    Portal(const Portal& Prototype);

public:
    virtual ~Portal();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
public:
    _bool Collider_Player(_float fTimeDelta);
private:
    HRESULT Ready_Components();
    HRESULT Ready_UI();
public:
    LEVEL Get_NextLevel() const {
        return m_eNextLevel;
    }

private:
    shared_ptr<Model> m_pModelCom = nullptr;
    shared_ptr<Shader> m_pShaderCom = nullptr;

private:
    wstring m_ModelComponentName = {};

private:
    LEVEL m_eNextLevel = LEVEL::GAMEPLAY;

public:
    map<string, shared_ptr<class GameObject>> m_pUI;

public:
    static unique_ptr<Portal> Create(
        ComPtr<ID3D11Device> pDevice,
        ComPtr<ID3D11DeviceContext> pContext);

    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END