#include "GameObject.h"

#include "Transform.h"
#include "GameInstance.h"


GameObject::GameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
}

GameObject::GameObject(const GameObject& Prototype)
    : m_pDevice{ Prototype.m_pDevice }
    , m_pContext{ Prototype.m_pContext }
{
}


HRESULT GameObject::Initialize_Prototype()
{


    return S_OK;
}

HRESULT GameObject::Initialize(void* pArg)
{
    m_pTransformCom = Transform::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTransformCom)
        return E_FAIL;

    if (nullptr == pArg)
        return S_OK;

    auto		pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    m_ObjectName = pDesc->m_strName;



    if (FAILED(m_pTransformCom->Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void GameObject::Priority_Update(_float fTimeDelta)
{
}

void GameObject::Update(_float fTimeDelta)
{
}

void GameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT GameObject::Render()
{
    return S_OK;
}

HRESULT GameObject::Add_Component(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, shared_ptr<Component>& pOut, void* pArg)
{
    if (nullptr != Find_Component(strComponentTag))
        return E_FAIL;

    auto	pComponent = dynamic_pointer_cast<Component>(CGameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));
    if (nullptr == pComponent)
        return E_FAIL;

    m_Components.emplace(strComponentTag, pComponent);

    pOut = pComponent;
    return S_OK;
}

shared_ptr<class Component> GameObject::Find_Component(const _wstring& strComponentTag)
{
    auto	iter = m_Components.find(strComponentTag);
    if (iter == m_Components.end())
        return nullptr;

    return iter->second;
}

void GameObject::RenderGUI() {
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

    if (!CGameInstance::Get().Mouse_Pressing(MOUSEKEYSTATE::DIM_RB)){
        if (ImGui::IsKeyPressed(ImGuiKey_T))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
    }
 
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];

    _float4x4 matrix = m_pTransformCom->GetWorldMatrix();
    ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&matrix), matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(&matrix));
    m_pTransformCom->Set_WorldMatrix(matrix);


    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }



    _float4x4 view, proj;
    CGameInstance::Get().Get_MainCameraMatrix(view,proj);
   


    ImGuizmo::Manipulate(reinterpret_cast<float*>(&view), reinterpret_cast<float*>(&proj), mCurrentGizmoOperation, mCurrentGizmoMode,
        reinterpret_cast<float*>(&matrix), NULL, NULL);
}



