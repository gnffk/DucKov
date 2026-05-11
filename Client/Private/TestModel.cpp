#include "Engine_Macro.h"
#include "TestModel.h"
#include "GameInstance.h"
#include "AABB_Collider.h"
#include "OBB_Collider.h"
TestModel::TestModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

TestModel::TestModel(const TestModel& Prototype)
	: GameObject{ Prototype }
{
}

TestModel::~TestModel()
{
	int a = 10;
}


HRESULT TestModel::Initialize_Prototype()
{

	return S_OK;
}

HRESULT TestModel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	m_pAABBCom->SetOwner(SHARED_THIS(TestModel).get());

	m_pModelCom->Set_Animation(0, false);
	return S_OK;
}

void TestModel::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLEND, SHARED_THIS(TestModel));
	CGameInstance::Get().Add_Collider(L"Player", m_pAABBCom.get());
}

void TestModel::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

}

void TestModel::Late_Update(_float fTimeDelta)
{

}

HRESULT TestModel::Render()
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

HRESULT TestModel::Ready_Components()
{


	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Model_Duck")));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Shader_Vtx_AnimFbx")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;


	m_pAABBCom = dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_OBB_Collider")));
	if (FAILED(__super::Add_Component(TEXT("Com_Collider"), m_pAABBCom)))
		return E_FAIL;
	


	

	return S_OK;
}

unique_ptr<TestModel> TestModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<TestModel>(new TestModel(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TestModel");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> TestModel::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new TestModel(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TestModel");
		return nullptr;
	}

	return pInstance;
}
void TestModel::IMGUITEST()
{
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