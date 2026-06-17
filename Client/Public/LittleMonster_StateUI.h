

#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END



NS_BEGIN(Client)

class LittleMonster_StateUI : public UIObject
{
private:
    LittleMonster_StateUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    LittleMonster_StateUI(const LittleMonster_StateUI& Prototype);

public:
    virtual ~LittleMonster_StateUI();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    HRESULT Render_BillboardRect(const wstring& strKey);

    void Set_BarRatio_Billboard(const wstring& strKey, const _float2& vBaseSize, float fRatio);

    void Update_HPBarVisual_Billboard();

public:
    void Set_HP(float fCurHP, float fMaxHP);

private:
    HRESULT Ready_Components();

    HRESULT Add_UIRect(const wstring& UIName, const wstring& strName, const wstring& strTextureTag, const _float2& vPos, const _float2& vSize, _float fDepth);

    HRESULT Render_UIRect(UI_RECT& UI);
    HRESULT Render_UIRect_ByKey(const wstring& strKey);

private:
    void Update_HPBar(_float fTimeDelta);
    void Update_HPBarVisual();

    void Set_BarRatio(const wstring& strKey, const _float2& vBasePos, const _float2& vBaseSize, float fRatio);

    float ClampFloat(float fValue, float fMin, float fMax);
    float LerpFloat(float fStart, float fEnd, float fRatio);

#ifdef _DEBUG
    void GUI_MonsterStateUI();
#endif
private:
    unordered_map<wstring, UI_RECT> m_UIRects;

public:
    void SetOwner(shared_ptr<GameObject> _pOwner) { m_pOwner = _pOwner; }

private:

    shared_ptr<Shader> m_pShaderCom = nullptr;
    shared_ptr<GameObject> m_pOwner = nullptr;
    shared_ptr<VIBuffer_Rect> m_pVIBufferCom = nullptr;
private:
    float m_fHPBarPaddingX = 4.f;
    float m_fHPBarPaddingY = 3.f;
private:
    _float2 m_vHPBarPos = { 180.f, 50.f };
    _float2 m_vHPBarSize = { 57.f, 18.f };

    float m_fMaxHP = 100.f;
    float m_fCurHP = 100.f;

    float m_fHpRatio = 1.f;
    float m_fDamageRatio = 1.f;
private:
    _float3 m_vBillboardCenter = { 0.f, 0.f, 0.f };

    // 이제 픽셀 크기가 아니라 월드 크기임
    _float2 m_vBillboardHPSize = { 1.2f, 0.12f };

    // 몬스터 머리 위 높이
    _float3 m_vBillboardOffset = { 0.f, 2.0f, 0.f };

    _bool m_bVisible = true;

private:
    _float3 m_vWorldOffset = { 0.f, 0.f, 0.f };   // 플레이어 머리 위 높이
    _float2 m_vScreenOffset = { 0.f, 0.f };      // 화면상 미세 조정
    _bool m_bOnScreen = false;



private:
    void Update_FollowOwner();
    HRESULT Render_BillboardRect();

private:
    HRESULT Save_UIRects(const wstring& strFilePath);
    HRESULT Load_UIRects(const wstring& strFilePath);
public:
    static unique_ptr<LittleMonster_StateUI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

    virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END