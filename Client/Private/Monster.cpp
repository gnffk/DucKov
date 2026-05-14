#include "Engine_Macro.h"
#include "Monster.h"
#include "GameInstance.h"
#include "AABB_Collider.h"
#include "OBB_Collider.h"
Monster::Monster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

Monster::Monster(const Monster& Prototype)
	: GameObject{ Prototype }
{
}

Monster::~Monster()
{
	//a.clear();
}


HRESULT Monster::Initialize_Prototype()
{

	return S_OK;
}

HRESULT Monster::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	m_pAABBCom->SetOwner(SHARED_THIS(Monster).get());

	//m_pModelCom->Set_Animation(0, false);
	return S_OK;
}

void Monster::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLEND, SHARED_THIS(Monster));
	CGameInstance::Get().Add_Collider(L"Player", m_pAABBCom.get());
}

void Monster::Update(_float fTimeDelta)
{
	if (m_pModelCom == nullptr) {
		return ;
	}
	m_pModelCom->Play_Animation(fTimeDelta);

}

void Monster::Late_Update(_float fTimeDelta)
{

}

HRESULT Monster::Render()
{
	
	IMGUITEST();

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View,Proj);

	_float4x4 World = m_pTransformCom->GetWorldMatrix();




	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;


	if (m_pModelCom == nullptr) {
		return S_OK;
	}
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

HRESULT Monster::Ready_Components()
{
	
	// Prototype_Com_Model_SK_Monster_Palicus
	if (Object_INFO.m_strPrototypeBaseName.size() != 0) {
		m_ModelComponentName = TEXT("Prototype_Com_Model_") + Object_INFO.m_strPrototypeBaseName;


		m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), m_ModelComponentName));
		if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
			return E_FAIL;
	}
	

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Com_Shader_Vtx_AnimFbx")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;
	

	m_pAABBCom = dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Com_OBB_Collider")));
	if (FAILED(__super::Add_Component(TEXT("Com_OBBCollider"), m_pAABBCom)))
		return E_FAIL;
	


	

	return S_OK;
}

HRESULT Monster::Set_Model(wstring m_strPrototypeBaseName) {
	Object_INFO.m_strPrototypeBaseName = m_strPrototypeBaseName;
	m_ModelComponentName = TEXT("Prototype_Com_Model_") + Object_INFO.m_strPrototypeBaseName;


	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), m_ModelComponentName));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

unique_ptr<Monster> Monster::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Monster>(new Monster(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Monster");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Monster::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Monster(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Monster");
		return nullptr;
	}

	return pInstance;
}
void Monster::IMGUITEST()
{
	if (CGameInstance::Get().GetSelectObject() != this || m_pModelCom== nullptr) {
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
}