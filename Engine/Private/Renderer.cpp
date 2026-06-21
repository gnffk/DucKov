#include "Renderer.h"

//1. 얼마나 밝아야 Bloom에 걸리는가 ? → Threshold
//2. Bloom이 얼마나 강하게 합쳐지는가 ? → FinalBloomStrength
//3. Blur가 얼마나 퍼지는가 ? → Blur kernel / downsample 단계
//4. 이펙트 자체가 얼마나 밝게 출력되는가 ? → Trail Shader Intensity

Renderer::Renderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : 
    m_pDevice{ pDevice }
,   m_pContext{ pContext }
{
}

Renderer::~Renderer()
{
}

HRESULT Renderer::Initialize()
{
    _float2     vViewportSize = CGameInstance::Get().Get_ViewportSize();

    /* For.Target_Diffuse */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Diffuse"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_Normal */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Normal"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Shade */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Shade"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Specular */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Specular"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Depth */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Depth"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_PickPos */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_PickPos"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_Scene */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Scene"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;


    uint32_t iSceneWidth = static_cast<uint32_t>(vViewportSize.x);
    uint32_t iSceneHeight = static_cast<uint32_t>(vViewportSize.y);

    uint32_t iDown0Width = max(1u, iSceneWidth / 2);
    uint32_t iDown0Height = max(1u, iSceneHeight / 2);

    uint32_t iDown1Width = max(1u, iDown0Width / 2);
    uint32_t iDown1Height = max(1u, iDown0Height / 2);

    uint32_t iDown2Width = max(1u, iDown1Width / 2);
    uint32_t iDown2Height = max(1u, iDown1Height / 2);

    /* For.Target_Bloom_Down0 : 1/2 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Bloom_Down0"),iDown0Width, iDown0Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Bloom_Down1 : 1/4 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Bloom_Down1"),iDown1Width, iDown1Height, DXGI_FORMAT_R16G16B16A16_FLOAT,_float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Bloom_Down2 : 1/8 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget( TEXT("Target_Bloom_Down2"), iDown2Width, iDown2Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Blur2_Temp : 1/8 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Blur2_Temp"),iDown2Width,iDown2Height,DXGI_FORMAT_R16G16B16A16_FLOAT,_float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Blur2 : 1/8 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Blur2"), iDown2Width,iDown2Height,DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Bloom_Up1 : 1/4 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Bloom_Up1"),iDown1Width,iDown1Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Blur1_Temp : 1/4 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Blur1_Temp"),iDown1Width, iDown1Height,DXGI_FORMAT_R16G16B16A16_FLOAT,_float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Blur1 : 1/4 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Blur1"), iDown1Width,iDown1Height,DXGI_FORMAT_R16G16B16A16_FLOAT,_float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Bloom_Up0 : 1/2 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Bloom_Up0"),iDown0Width, iDown0Height, DXGI_FORMAT_R16G16B16A16_FLOAT,_float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Blur0_Temp : 1/2 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Blur0_Temp"),iDown0Width,iDown0Height, DXGI_FORMAT_R16G16B16A16_FLOAT,_float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Blur0 : 1/2 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Blur0"),iDown0Width,iDown0Height,DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_BlurDown1_Temp : 1/4 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_BlurDown1_Temp"),iDown1Width,iDown1Height, DXGI_FORMAT_R16G16B16A16_FLOAT,_float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_BlurDown1 : 1/4 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_BlurDown1"),iDown1Width,iDown1Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_BlurDown0_Temp : 1/2 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_BlurDown0_Temp"), iDown0Width, iDown0Height,DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_BlurDown0 : 1/2 */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_BlurDown0"), iDown0Width, iDown0Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;


    /* For.MRT_GameObject */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Depth"))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_PickPos"))))
        return E_FAIL;

    /* For.MRT_LightAcc */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
        return E_FAIL;


    /* For.MRT_Scene */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Scene"), TEXT("Target_Scene"))))
        return E_FAIL;

    /* For.MRT_Bloom_Down0 */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Bloom_Down0"), TEXT("Target_Bloom_Down0"))))
        return E_FAIL;

    /* For.MRT_Bloom_Down1 */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Bloom_Down1"), TEXT("Target_Bloom_Down1"))))
        return E_FAIL;

    /* For.MRT_Bloom_Down2 */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Bloom_Down2"), TEXT("Target_Bloom_Down2"))))
        return E_FAIL;

    /* For.MRT_Blur2_Temp */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Blur2_Temp"),TEXT("Target_Blur2_Temp"))))
        return E_FAIL;

    /* For.MRT_Blur2 */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Blur2"),TEXT("Target_Blur2"))))
        return E_FAIL;

    /* For.MRT_Bloom_Up1 */
    if (FAILED(CGameInstance::Get().Add_MRT( TEXT("MRT_Bloom_Up1"),TEXT("Target_Bloom_Up1"))))
        return E_FAIL;

    /* For.MRT_Blur1_Temp */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Blur1_Temp"),TEXT("Target_Blur1_Temp"))))
        return E_FAIL;

    /* For.MRT_Blur1 */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Blur1"), TEXT("Target_Blur1"))))
        return E_FAIL;

    /* For.MRT_Bloom_Up0 */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Bloom_Up0"),TEXT("Target_Bloom_Up0"))))
        return E_FAIL;

    /* For.MRT_Blur0_Temp */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Blur0_Temp"),TEXT("Target_Blur0_Temp"))))
        return E_FAIL;

    /* For.MRT_Blur0 */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Blur0"), TEXT("Target_Blur0"))))
        return E_FAIL;

    /* For.MRT_BlurDown1_Temp */
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_BlurDown1_Temp"),TEXT("Target_BlurDown1_Temp"))))
        return E_FAIL;

    /* For.MRT_BlurDown1 */
    if (FAILED(CGameInstance::Get().Add_MRT( TEXT("MRT_BlurDown1"),TEXT("Target_BlurDown1"))))
        return E_FAIL;

    /* For.MRT_BlurDown0_Temp */
    if (FAILED(CGameInstance::Get().Add_MRT( TEXT("MRT_BlurDown0_Temp"),TEXT("Target_BlurDown0_Temp"))))
        return E_FAIL;

    /* For.MRT_BlurDown0 */
    if (FAILED(CGameInstance::Get().Add_MRT( TEXT("MRT_BlurDown0"), TEXT("Target_BlurDown0"))))
        return E_FAIL;


    m_pVIBuffer = VIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    m_pShader = Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Deferred.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(vViewportSize.x, vViewportSize.y, 1.f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(vViewportSize.x, vViewportSize.y, 0.f, 1.f));

//#ifdef _DEBUG
//    _float fDebugW = 160.f;
//    _float fDebugH = 90.f;
//    _float fGap = 10.f;
//
//    if (FAILED(CGameInstance::Get().Ready_RT_Debug(
//        TEXT("Target_Bloom_Down0"),
//        10.f,
//        50.f,
//        fDebugW,
//        fDebugH)))
//        return E_FAIL;
//
//    if (FAILED(CGameInstance::Get().Ready_RT_Debug(
//        TEXT("Target_Bloom_Down1"),
//        10.f,
//        50.f + (fDebugH + fGap) * 1.f,
//        fDebugW,
//        fDebugH)))
//        return E_FAIL;
//
//    if (FAILED(CGameInstance::Get().Ready_RT_Debug(
//        TEXT("Target_Bloom_Down2"),
//        10.f,
//        50.f + (fDebugH + fGap) * 2.f,
//        fDebugW,
//        fDebugH)))
//        return E_FAIL;
//
//    if (FAILED(CGameInstance::Get().Ready_RT_Debug(
//        TEXT("Target_Bloom_Up1"),
//        10.f,
//        50.f + (fDebugH + fGap) * 3.f,
//        fDebugW,
//        fDebugH)))
//        return E_FAIL;
//
//    if (FAILED(CGameInstance::Get().Ready_RT_Debug(
//        TEXT("Target_Bloom_Up0"),
//        10.f,
//        50.f + (fDebugH + fGap) * 4.f,
//        fDebugW,
//        fDebugH)))
//        return E_FAIL;
//
//
//
//
//
//#endif


    return S_OK;
}


HRESULT Renderer::Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<GameObject> pRenderObject)
{
    if (eRenderGroup >= RENDERGROUP::END ||
        nullptr == pRenderObject)
        return E_FAIL;

    m_RenderObjects[ETOUI(eRenderGroup)].push_back(pRenderObject);

    return S_OK;

}

HRESULT Renderer::Draw()
{
    if (CGameInstance::Get().Key_Down(DIK_1)) {
        debugRender = 1;
    }
    if (CGameInstance::Get().Key_Down(DIK_2)) {
        debugRender = 2;
    }
    if (CGameInstance::Get().Key_Down(DIK_3)) {
        debugRender = 3;
    }
    if (CGameInstance::Get().Key_Down(DIK_4)) {
        debugRender = 4;
    }
    if (CGameInstance::Get().Key_Down(DIK_5)) {
        debugRender = 5;
    }
    if (CGameInstance::Get().Key_Down(DIK_6)) {
        debugRender = 6;
    }
    if (CGameInstance::Get().Key_Down(DIK_7)) {
        debugRender = 7;
    }
    if (CGameInstance::Get().Key_Down(DIK_8)) {
        debugRender = 8;
    }


    if (FAILED(Render_NonBlend()))
        return E_FAIL;

    if (FAILED(Render_Lights()))
        return E_FAIL;

    if (FAILED(Render_Scene()))
        return E_FAIL;


    if (FAILED(Render_BloomDownSamples()))
        return E_FAIL;

    if (FAILED(Render_Blur2()))
        return E_FAIL;

    if (FAILED(Render_UpSample1()))
        return E_FAIL;

    if (FAILED(Render_Blur1()))
        return E_FAIL;

    if (FAILED(Render_UpSample2()))
        return E_FAIL;

    if (FAILED(Render_Blur3()))
        return E_FAIL;

    if (FAILED(Render_Final()))
        return E_FAIL;

    
    if (FAILED(Render_Effect()))
        return E_FAIL;
    

    if (FAILED(Render_UI()))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(Render_DEBUG()))
        return E_FAIL;
#endif

    return S_OK;
}
#ifdef _DEBUG

HRESULT Renderer::Render_DEBUG()
{


    _float4x4   WorldMatrix = {};

    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());


    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_GameObject"), m_pShader, "g_Texture", m_pVIBuffer)))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_Bloom_Down0"), m_pShader, "g_Texture", m_pVIBuffer)))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_Bloom_Down1"), m_pShader, "g_Texture", m_pVIBuffer)))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_Bloom_Down2"), m_pShader, "g_Texture", m_pVIBuffer)))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_LightAcc"), m_pShader, "g_Texture", m_pVIBuffer)))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_Bloom_Up1"), m_pShader, "g_Texture", m_pVIBuffer)))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_Bloom_Up0"), m_pShader, "g_Texture", m_pVIBuffer)))
        return E_FAIL;

    return S_OK;
}

#endif
HRESULT Renderer::Render_Priority()
{
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::PRIORITY)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();
    }

    m_RenderObjects[ETOUI(RENDERGROUP::PRIORITY)].clear();

    return S_OK;
}

HRESULT Renderer::Render_NonBlend()
{
    /* Diffuse + Normal + Depth */
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_GameObject"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::NONBLEND)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();
    }

    m_RenderObjects[ETOUI(RENDERGROUP::NONBLEND)].clear();

    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT Renderer::Render_Blend()
{
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::BLEND)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();
    }

    m_RenderObjects[ETOUI(RENDERGROUP::BLEND)].clear();

    return S_OK;
}

HRESULT Renderer::Render_Lights()
{
    /* Shade */
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);



    _float4x4 Proj, View;
    _float4x4 InvProj, InvView;
    _float4   CameraPosition;
    CGameInstance::Get().Get_MainCamerwaProjectionMatrix(Proj);
    CGameInstance::Get().Get_MainCamerwaViewMatrix(View);
    CGameInstance::Get().Get_MainCameraPosition(CameraPosition);
    XMMATRIX matProj = XMLoadFloat4x4(&Proj);
    XMMATRIX matView = XMLoadFloat4x4(&View);

    XMMATRIX matInvProj = XMMatrixInverse(nullptr, matProj);
    XMMATRIX matInvView = XMMatrixInverse(nullptr, matView);

    XMStoreFloat4x4(&InvProj, matInvProj);
    XMStoreFloat4x4(&InvView, matInvView);

    m_pShader->Bind_Matrix("g_ProjMatrixInverse", &InvProj);
    m_pShader->Bind_Matrix("g_ViewMatrixInverse", &InvView);

    m_pShader->Bind_RawValue("g_vCamPosition", &CameraPosition, sizeof(_float4));

    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Render_Lights(m_pShader, m_pVIBuffer)))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT Renderer::Render_Combined()
{



    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::COMBINED))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;



    return S_OK;
}

HRESULT Renderer::Render_NonLights()
{
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::NONLIGHT)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();
    }

    m_RenderObjects[ETOUI(RENDERGROUP::NONLIGHT)].clear();

    return S_OK;
}

HRESULT Renderer::Render_UI()
{
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::UI)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();
    }

    m_RenderObjects[ETOUI(RENDERGROUP::UI)].clear();

    return S_OK;
}

HRESULT Renderer::Render_Effect()
{
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::EFFECT)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();
    }

    m_RenderObjects[ETOUI(RENDERGROUP::EFFECT)].clear();
    return S_OK;
}

HRESULT Renderer::Render_Final()
{
    if (debugRender == 1) {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Scene"), m_pShader, "g_Texture")))
            return E_FAIL;
    }
    else if (debugRender == 2) {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Bloom_Down0"), m_pShader, "g_Texture")))
            return E_FAIL;

    }
    else if (debugRender == 3) {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Bloom_Down1"), m_pShader, "g_Texture")))
            return E_FAIL;

    }
    else if (debugRender == 4) {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Bloom_Down2"), m_pShader, "g_Texture")))
            return E_FAIL;

    }
    else if (debugRender ==5) {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Blur2"), m_pShader, "g_Texture")))
            return E_FAIL;

    }
    else if (debugRender ==6) {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Bloom_Up1"), m_pShader, "g_Texture")))
            return E_FAIL;

    }
    else if (debugRender ==7) {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Bloom_Up0"), m_pShader, "g_Texture")))
            return E_FAIL;

    }
    else if (debugRender == 8) {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Scene"),m_pShader,"g_Texture")))
            return E_FAIL;

        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Blur0"),m_pShader,"g_BloomTexture")))
            return E_FAIL;

        _float fFinalBloomStrength = 0.6f;
        m_pShader->Bind_RawValue("g_fFinalBloomStrength",&fFinalBloomStrength,sizeof(_float));
    }

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (debugRender == 8) {
        if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::COMPOSITE))))
            return E_FAIL;
    }
    else {
        if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::FINAL))))
            return E_FAIL;
    }


    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT Renderer::Render_Scene()
{
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_Scene"))))
        return E_FAIL;

    if (FAILED(Render_Priority()))
        return E_FAIL;

    if (FAILED(Render_Combined()))
        return E_FAIL;

    if (FAILED(Render_Blend()))
        return E_FAIL;

    if (FAILED(Render_NonLights()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT Renderer::Render_Combined_InScene()
{
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::COMBINED))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

unique_ptr<Renderer> Renderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto		pInstance = unique_ptr<Renderer>(new Renderer(pDevice, pContext));

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : Renderer");
        return nullptr;
    }

    return pInstance;
}


HRESULT Renderer::Render_BloomDownSamples()
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float2 vSceneTexelSize =
    {
        1.f / vViewportSize.x,
        1.f / vViewportSize.y
    };

    _float2 vDown0Size =
    {
        vViewportSize.x * 0.5f,
        vViewportSize.y * 0.5f
    };

    _float2 vDown0TexelSize =
    {
        1.f / vDown0Size.x,
        1.f / vDown0Size.y
    };

    _float2 vDown1Size =
    {
        vDown0Size.x * 0.5f,
        vDown0Size.y * 0.5f
    };

    _float2 vDown1TexelSize =
    {
        1.f / vDown1Size.x,
        1.f / vDown1Size.y
    };

    // 1. Target_Scene -> Target_Bloom_Down0
    // Bright 추출 + 4x4 DownSample
    if (FAILED(Render_DownSample(TEXT("Target_Scene"), TEXT("MRT_Bloom_Down0"),DEFERRED::DOWNSAMPLE_BRIGHT_4X4, vSceneTexelSize)))
        return E_FAIL;

    // 2. Target_Bloom_Down0 -> Target_Bloom_Down1
    // 6x6 DownSample
    if (FAILED(Render_DownSample(TEXT("Target_Bloom_Down0"),TEXT("MRT_Bloom_Down1"),DEFERRED::DOWNSAMPLE_6X6,vDown0TexelSize)))
        return E_FAIL;

    // 3. Target_Bloom_Down1 -> Target_Bloom_Down2
    // 6x6 DownSample
    if (FAILED(Render_DownSample( TEXT("Target_Bloom_Down1"),TEXT("MRT_Bloom_Down2"), DEFERRED::DOWNSAMPLE_6X6,vDown1TexelSize)))
        return E_FAIL;


    return S_OK;
}

HRESULT Renderer::Render_DownSample(const _wstring& strSrcTarget, const _wstring& strDstMRT,DEFERRED ePass,const _float2& vSrcTexelSize)
{

    if (FAILED(CGameInstance::Get().Begin_MRT(strDstMRT, false)))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(strSrcTarget,m_pShader,"g_Texture")))
        return E_FAIL;

    m_pShader->Bind_RawValue("g_vTexelSize",&vSrcTexelSize,sizeof(_float2));

    _float fBloomThreshold = 0.8f;
    m_pShader->Bind_RawValue("g_fBloomThreshold",&fBloomThreshold,sizeof(_float));

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(ePass))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}


HRESULT Renderer::Render_Blur1()
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float2 vDown0Size =
    {
        vViewportSize.x * 0.5f,
        vViewportSize.y * 0.5f
    };

    _float2 vDown1Size =
    {
        vDown0Size.x * 0.5f,
        vDown0Size.y * 0.5f
    };

    _float2 vDown1TexelSize =
    {
        1.f / vDown1Size.x,
        1.f / vDown1Size.y
    };

    // 1. Horizontal Blur
    if (FAILED(Render_BlurPass(TEXT("Target_Bloom_Up1"),TEXT("MRT_Blur1_Temp"),vDown1TexelSize,1)))
        return E_FAIL;

    // 2. Vertical Blur
    if (FAILED(Render_BlurPass(TEXT("Target_Blur1_Temp"), TEXT("MRT_Blur1"),vDown1TexelSize,0)))
        return E_FAIL;

    return S_OK;
}

HRESULT Renderer::Render_Blur2()
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float2 vDown0Size =
    {
        vViewportSize.x * 0.5f,
        vViewportSize.y * 0.5f
    };

    _float2 vDown1Size =
    {
        vDown0Size.x * 0.5f,
        vDown0Size.y * 0.5f
    };

    _float2 vDown2Size =
    {
        vDown1Size.x * 0.5f,
        vDown1Size.y * 0.5f
    };

    _float2 vDown2TexelSize =
    {
        1.f / vDown2Size.x,
        1.f / vDown2Size.y
    };

    // 1. Horizontal Blur
    if (FAILED(Render_BlurPass(TEXT("Target_Bloom_Down2"),TEXT("MRT_Blur2_Temp"),vDown2TexelSize,1)))
        return E_FAIL;

    // 2. Vertical Blur
    if (FAILED(Render_BlurPass(TEXT("Target_Blur2_Temp"),TEXT("MRT_Blur2"),vDown2TexelSize, 0)))
        return E_FAIL;

    return S_OK;
}

HRESULT Renderer::Render_Blur3()
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float2 vDown0Size =
    {
        vViewportSize.x * 0.5f,
        vViewportSize.y * 0.5f
    };

    _float2 vDown0TexelSize =
    {
        1.f / vDown0Size.x,
        1.f / vDown0Size.y
    };

    // 1. Target_Bloom_Up0 -> Target_Blur0_Temp
    // Horizontal Blur
    if (FAILED(Render_BlurPass(TEXT("Target_Bloom_Up0"),TEXT("MRT_Blur0_Temp"),vDown0TexelSize,1)))
        return E_FAIL;

    // 2. Target_Blur0_Temp -> Target_Blur0
    // Vertical Blur
    if (FAILED(Render_BlurPass(TEXT("Target_Blur0_Temp"), TEXT("MRT_Blur0"),vDown0TexelSize,0)))
        return E_FAIL;

    return S_OK;
}

HRESULT Renderer::Render_BlurPass(const _wstring& strSrcTarget,const _wstring& strDstMRT,const _float2& vSrcTexelSize, int iHorizontal)
{
    if (FAILED(CGameInstance::Get().Begin_MRT(strDstMRT, false)))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(strSrcTarget,m_pShader,"g_Texture")))
        return E_FAIL;

    m_pShader->Bind_RawValue("g_vTexelSize",&vSrcTexelSize,sizeof(_float2));

    m_pShader->Bind_RawValue( "g_iHorizontal", &iHorizontal,sizeof(int));

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::BLUR))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT Renderer::Render_UpSample1()
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float2 vDown0Size =
    {
        vViewportSize.x * 0.5f,
        vViewportSize.y * 0.5f
    };

    _float2 vDown1Size =
    {
        vDown0Size.x * 0.5f,
        vDown0Size.y * 0.5f
    };

    _float2 vDown2Size =
    {
        vDown1Size.x * 0.5f,
        vDown1Size.y * 0.5f
    };

    _float2 vDown2TexelSize =
    {
        1.f / vDown2Size.x,
        1.f / vDown2Size.y
    };

    // Target_Blur2를 Down1 크기로 업샘플링하고
    // Target_Bloom_Down1과 합쳐서 Target_Bloom_Up1에 저장
    if (FAILED(Render_UpSampleBlend(TEXT("Target_Blur2"), TEXT("Target_Bloom_Down1"),TEXT("MRT_Bloom_Up1"),vDown2TexelSize)))
        return E_FAIL;

    return S_OK;
}

HRESULT Renderer::Render_UpSample2()
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float2 vDown0Size =
    {
        vViewportSize.x * 0.5f,
        vViewportSize.y * 0.5f
    };

    _float2 vDown1Size =
    {
        vDown0Size.x * 0.5f,
        vDown0Size.y * 0.5f
    };

    _float2 vDown1TexelSize =
    {
        1.f / vDown1Size.x,
        1.f / vDown1Size.y
    };

    // Target_Blur1을 Down0 크기로 업샘플링하고
    // Target_Bloom_Down0과 합쳐서 Target_Bloom_Up0에 저장
    if (FAILED(Render_UpSampleBlend(TEXT("Target_Blur1"),TEXT("Target_Bloom_Down0"),TEXT("MRT_Bloom_Up0"),vDown1TexelSize)))
        return E_FAIL;

    return S_OK;
}

HRESULT Renderer::Render_UpSamplePass(const _wstring& strSrcTarget, const _wstring& strDstMRT, const _float2& vSrcTexelSize)
{
    if (FAILED(CGameInstance::Get().Begin_MRT(strDstMRT, false)))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(strSrcTarget, m_pShader,"g_Texture")))
        return E_FAIL;

    m_pShader->Bind_RawValue("g_vTexelSize",&vSrcTexelSize,sizeof(_float2));

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::UPSAMPLE_6x6))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}


HRESULT Renderer::Render_UpSampleBlend(const _wstring& strLowTarget, const _wstring& strBlendTarget,const _wstring& strDstMRT,const _float2& vLowTexelSize)
{
    if (FAILED(CGameInstance::Get().Begin_MRT(strDstMRT, false)))
        return E_FAIL;

    // 업샘플링할 낮은 해상도 텍스처
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource( strLowTarget,m_pShader,"g_Texture")))
        return E_FAIL;

    // 합칠 현재 단계 텍스처
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(strBlendTarget, m_pShader, "g_BlendTexture")))
        return E_FAIL;

    m_pShader->Bind_RawValue( "g_vTexelSize", &vLowTexelSize,sizeof(_float2));

    _float fBloomStrength = 0.75f;
    m_pShader->Bind_RawValue("g_fBloomStrength", &fBloomStrength,sizeof(_float));

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::UPSAMPLE_BLEND_6X6))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}

