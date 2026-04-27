#include "BaseCollider.h"

BaseCollider::BaseCollider(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext) : Component{ pDevice, pContext }
{
}

BaseCollider::~BaseCollider()
{
}

HRESULT BaseCollider::Initialize_Prototype() {

	return S_OK;
}
HRESULT BaseCollider::Initialize(void* pArg) {
	return S_OK;
}


HRESULT BaseCollider::Bind_Resources()
{



    return S_OK;
}

HRESULT BaseCollider::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> m_batch)
{

  
    return S_OK;

}

void BaseCollider::GUI_ColliderExtend()
{
	if (CGameInstance::Get().GetSelectObject() == m_Owner) {
        ImGui::SeparatorText("Collider Settings");


        ImGui::Text("Center");

        // 그래프(드래그)
        ImGui::DragFloat3("Center Drag", (float*)&m_Center, 0.1f);

        // 숫자 입력
        ImGui::InputFloat3("Center Input", (float*)&m_Center);


        ImGui::Spacing();



        ImGui::Text("Extents");

        // 그래프(드래그)
        ImGui::DragFloat3("Extents Drag", (float*)&m_Extend, 0.1f, 0.0f, 100.0f);

        // 숫자 입력
        ImGui::InputFloat3("Extents Input", (float*)&m_Extend);


        ImGui::Spacing();


        if (ImGui::Button("Reset Collider"))
        {
            m_Extend = { 0.5f, 0.5f, 0.5f };
            m_Center = { 0.f, 0.f, 0.f };
        }
	}

}
