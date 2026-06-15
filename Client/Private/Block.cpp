#include "Engine_Macro.h"
#include "Block.h"
#include "GameInstance.h"
#include "AABB_Collider.h"
#include "OBB_Collider.h"
#include "Sphere_Collider.h"
Block::Block(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

Block::Block(const Block& Prototype)
	: GameObject{ Prototype }
{
}

Block::~Block()
{
	//a.clear();
}


HRESULT Block::Initialize_Prototype()
{

	return S_OK;
}

HRESULT Block::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//if (m_pAABBCom != nullptr) {
	//	m_pAABBCom->SetOwner(SHARED_THIS(Obstacle).get());
	//}


	//m_pModelCom->Set_Animation(0, false);
	return S_OK;
}

void Block::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Block));

	for (int type = 0; type < (int)COLLIDER::COLLIDER_END; ++type)
	{
		auto& colliderList = m_pColliderComs[type];

		for (size_t i = 0; i < colliderList.size(); ++i)
		{

			CGameInstance::Get().Add_Collider(colliderList[i]->Get_GroupTag(), colliderList[i].get());

		}
	}


}

void Block::Update(_float fTimeDelta)
{



}

void Block::Late_Update(_float fTimeDelta)
{

}

HRESULT Block::Render()
{
#ifdef _DEBUG
	IMGUITEST();

#endif
	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

	_float4x4 World = m_pTransformCom->GetWorldMatrix();




	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;



	uint32_t	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (uint32_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", (uint32_t)i, (uint32_t)ETOUI(TEXTURETYPE::DIFFUSE), 0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (uint32_t)i)))
			return E_FAIL;

		//if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_Normals, 0)))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT Block::Ready_Components()
{
	__super::Clear_Compnent();

	m_ModelComponentName = TEXT("Prototype_Com_Model_") + Object_INFO.m_strPrototypeBaseName;


	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), m_ModelComponentName));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;


	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_FBX_Tex")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	//if (Object_INFO.m_bCollider) {
	//	m_pAABBCom = dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_OBB_Collider")));
	//	if (FAILED(__super::Add_Component(TEXT("Com_OBBCollider"), m_pAABBCom)))
	//		return E_FAIL;
	//}







	return S_OK;
}


unique_ptr<Block> Block::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Block>(new Block(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Block");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Block::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Block(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Block");
		return nullptr;
	}

	return pInstance;
}
void Block::IMGUITEST()
{
	if (CGameInstance::Get().GetSelectObject() != this || m_pModelCom == nullptr) {
		return;
	}

	//-----------------------------------------------Collider-------------------------------------------------
	ImGui::Begin("Collider");

	static int selectedCollider = -1;

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 10.f));

	ImGui::SeparatorText("Create Collider");

	auto CreateUniqueColliderName = [&](const wstring& baseName)
		{
			wstring finalName = baseName;

			int index = 1;

			while (true)
			{
				auto iter =
					m_Components.find(finalName);

				if (iter == m_Components.end())
					break;

				finalName =
					baseName + L"_" + to_wstring(index);

				++index;
			}

			return finalName;
		};

	if (ImGui::Selectable("AABB Collider", selectedCollider == 0))
	{
		selectedCollider = 0;

		auto pAABBCom =
			dynamic_pointer_cast<BaseCollider>(
				CGameInstance::Get().Clone_Prototype(
					CGameInstance::Get().Get_Level(),
					TEXT("Prototype_Com_AABB_Collider")));
		pAABBCom->Set_Tag(COLLIDER::COLLIDER_AABB);
		wstring componentName =
			CreateUniqueColliderName(L"Com_AABBCollider");


		__super::Add_Component(componentName, pAABBCom);

		pAABBCom->SetOwner(SHARED_THIS(Block).get());
		pAABBCom->Set_GroupTag(L"Block");
		m_pColliderComs[(int)COLLIDER::COLLIDER_AABB].push_back(pAABBCom);
	}

	if (ImGui::Selectable("OBB Collider", selectedCollider == 1))
	{
		selectedCollider = 1;

		auto pOBBCom =
			dynamic_pointer_cast<BaseCollider>(
				CGameInstance::Get().Clone_Prototype(
					CGameInstance::Get().Get_Level(),
					TEXT("Prototype_Com_OBB_Collider")));

		pOBBCom->Set_Tag(COLLIDER::COLLIDER_OBB);
		pOBBCom->Set_GroupTag(L"Block");
		wstring componentName =
			CreateUniqueColliderName(L"Com_OBBCollider");

		__super::Add_Component(
			componentName,
			pOBBCom);

		pOBBCom->SetOwner(
			SHARED_THIS(Block).get());

		m_pColliderComs[(int)COLLIDER::COLLIDER_OBB]
			.push_back(pOBBCom);
	}


	if (ImGui::Selectable("Sphere Collider", selectedCollider == 2))
	{
		selectedCollider = 2;

		auto pSphereCom =
			dynamic_pointer_cast<BaseCollider>(
				CGameInstance::Get().Clone_Prototype(
					CGameInstance::Get().Get_Level(),
					TEXT("Prototype_Com_Sphere_Collider")));
		pSphereCom->Set_Tag(COLLIDER::COLLIDER_SPHERE);
		wstring componentName =
			CreateUniqueColliderName(L"Com_Sphere_Collider");


		__super::Add_Component(
			componentName,
			pSphereCom);

		pSphereCom->SetOwner(
			SHARED_THIS(Block).get());
		pSphereCom->Set_GroupTag(L"Block");
		m_pColliderComs[(int)COLLIDER::COLLIDER_SPHERE]
			.push_back(pSphereCom);
	}

	ImGui::Spacing();
	ImGui::Separator();

	ImGui::TextDisabled("Click a collider type to add it.");


	// =====================================================
// Collider Settings
// =====================================================

	ImGui::Spacing();
	ImGui::SeparatorText("Collider Settings");
	for (int type = 0; type < (int)COLLIDER::COLLIDER_END; ++type)
	{
		auto& colliderList = m_pColliderComs[type];

		for (size_t i = 0; i < colliderList.size(); ++i)
		{
			ImGui::PushID(type * 1000 + (int)i);

			auto& collider = colliderList[i];

			if (collider == nullptr)
			{
				ImGui::PopID();
				continue;
			}

			string colliderTypeName;

			switch ((COLLIDER)type)
			{
			case COLLIDER::COLLIDER_AABB:
				colliderTypeName = "AABB";
				break;

			case COLLIDER::COLLIDER_OBB:
				colliderTypeName = "OBB";
				break;

			case COLLIDER::COLLIDER_SPHERE:
				colliderTypeName = "SPHERE";
				break;
			}

			string headerName =
				colliderTypeName + " Collider " + std::to_string(i);

			if (ImGui::CollapsingHeader(headerName.c_str()))
			{
				collider->GUI_ColliderExtend();
			}

			ImGui::PopID();
		}
	}
	ImGui::PopStyleVar(2);

	ImGui::End();

}