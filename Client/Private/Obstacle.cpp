#include "Engine_Macro.h"
#include "Obstacle.h"
#include "GameInstance.h"
#include "AABB_Collider.h"
#include "OBB_Collider.h"
#include "Sphere_Collider.h"
Obstacle::Obstacle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

Obstacle::Obstacle(const Obstacle& Prototype)
	: GameObject{ Prototype }
{
}

Obstacle::~Obstacle()
{
	//a.clear();
}


HRESULT Obstacle::Initialize_Prototype()
{

	return S_OK;
}

HRESULT Obstacle::Initialize(void* pArg)
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

void Obstacle::Priority_Update(_float fTimeDelta)
{
	if (m_ModelComponentName != L"Prototype_Com_Model_SM_MeshV2") {

		CGameInstance::Get().Add_RenderObject(RENDERGROUP::SHADOW, SHARED_THIS(Obstacle));
	}
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Obstacle));
	
	for (int type = 0; type < (int)COLLIDER::COLLIDER_END; ++type)
	{
		auto& colliderList = m_pColliderComs[type];

		for (size_t i = 0; i < colliderList.size(); ++i)
		{

			CGameInstance::Get().Add_Collider(colliderList[i]->Get_GroupTag(), colliderList[i].get());

		}
	}

		
}

void Obstacle::Update(_float fTimeDelta)
{



}

void Obstacle::Late_Update(_float fTimeDelta)
{

}

HRESULT Obstacle::Render()
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

HRESULT Obstacle::Render_Shadow()
{
	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

	_float4x4 World = m_pTransformCom->GetWorldMatrix();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_ShadowLightTransform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_ShadowLightTransform(D3DTS::PROJ))))
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

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;


		m_pModelCom->Render(i);
	}

	return S_OK;
}
HRESULT Obstacle::Ready_Components()
{
	__super::Clear_Compnent();

	m_ModelComponentName = TEXT("Prototype_Com_Model_") + Object_INFO.m_strPrototypeBaseName;


	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), m_ModelComponentName));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;


	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Obstacle")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	//if (Object_INFO.m_bCollider) {
	//	m_pAABBCom = dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_OBB_Collider")));
	//	if (FAILED(__super::Add_Component(TEXT("Com_OBBCollider"), m_pAABBCom)))
	//		return E_FAIL;
	//}







	return S_OK;
}


unique_ptr<Obstacle> Obstacle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Obstacle>(new Obstacle(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Obstacle");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Obstacle::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Obstacle(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Obstacle");
		return nullptr;
	}

	return pInstance;
}
void Obstacle::IMGUITEST()
{
	if (CGameInstance::Get().GetSelectObject() != this || m_pModelCom == nullptr) {
		return;
	}



	uint32_t iNumAnim = m_pModelCom->Get_NumAnimation();

	static int  currentAnim = 0;
	static bool bLoop = true;

	ImGui::Begin("Animation Debug");

	// 애니메이션 선택
	ImGui::SliderInt("Anim Index", &currentAnim, 0, (int)iNumAnim - 1);

	// 루프 여부
	ImGui::Checkbox("Loop", &bLoop);

	// 재생 버튼
	if (ImGui::Button("Play Animation"))
	{
		m_pModelCom->Set_Animation(currentAnim, bLoop);
	}

	ImGui::SameLine();

	// 정지 버튼
	if (ImGui::Button("Stop Animation"))
	{
		// 엔진 구조에 맞게 수정
		//m_pModelCom->Set_PlayAnimation(FALSE);

		// 또는
		// m_pModelCom->Stop_Animation();

		// 또는
		// m_pModelCom->Set_AnimationSpeed(0.f);
	}

	ImGui::Separator();

	// 리스트 형태
	for (uint32_t i = 0; i < iNumAnim; ++i)
	{
		ImGui::PushID(i);

		bool bSelected = (currentAnim == i);

		if (ImGui::Selectable(("Animation " + std::to_string(i)).c_str(), bSelected))
		{
			currentAnim = i;
		}

		ImGui::SameLine();

		if (ImGui::SmallButton("Play"))
		{
			m_pModelCom->Set_Animation(i, bLoop);
		}

		ImGui::SameLine();

		std::string stopLabel = "Stop##" + std::to_string(i);

		if (ImGui::SmallButton(stopLabel.c_str()))
		{
			//m_pModelCom->Set_PlayAnimation(FALSE);

			// 또는
			// m_pModelCom->Stop_Animation();
		}

		ImGui::PopID();
	}

	ImGui::End();

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


		__super::Add_Component(componentName,pAABBCom);

		pAABBCom->SetOwner(SHARED_THIS(Obstacle).get());
		pAABBCom->Set_GroupTag(L"Obstacle");
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
		pOBBCom->Set_GroupTag(L"Obstacle");
		wstring componentName =
			CreateUniqueColliderName(L"Com_OBBCollider");

		__super::Add_Component(
			componentName,
			pOBBCom);

		pOBBCom->SetOwner(
			SHARED_THIS(Obstacle).get());

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
			SHARED_THIS(Obstacle).get());
		pSphereCom->Set_GroupTag(L"Obstacle");
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