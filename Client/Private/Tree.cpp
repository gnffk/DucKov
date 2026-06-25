#include "Engine_Macro.h"
#include "Tree.h"
#include "GameInstance.h"
#include "Terrain.h"
Tree::Tree(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

Tree::Tree(const Tree& Prototype)
	: GameObject{ Prototype }
{
}

Tree::~Tree()
{
	//a.clear();
}


HRESULT Tree::Initialize_Prototype()
{

	return S_OK;
}

HRESULT Tree::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;

}

void Tree::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Tree));
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::SHADOW, SHARED_THIS(Tree));



}

void Tree::Update(_float fTimeDelta)
{

	Update_TreeBrush(fTimeDelta);
	
}

void Tree::Late_Update(_float fTimeDelta)
{

}

HRESULT Tree::Render()
{
	if (m_pModelCom == nullptr || m_pShaderCom == nullptr)
		return E_FAIL;

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);



	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;

	uint32_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (uint32_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom,"g_DiffuseTexture",i,ETOUI(TEXTURETYPE::DIFFUSE),0)))
		{
			return E_FAIL;
		}
	
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT Tree::Render_Shadow()
{
	if (m_pModelCom == nullptr || m_pShaderCom == nullptr)
		return E_FAIL;

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);



	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_ShadowLightTransform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_ShadowLightTransform(D3DTS::PROJ))))
		return E_FAIL;


	uint32_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (uint32_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, ETOUI(TEXTURETYPE::DIFFUSE), 0)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}
HRESULT Tree::Ready_Components()
{
	__super::Clear_Compnent();

	m_pModelCom =dynamic_pointer_cast<InstanceModel>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Model_Tree")));

	if (m_pModelCom == nullptr)
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;


	m_pShaderCom =dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_Vtx_Instance_Tex")));

	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT Tree::Render_GUI()
{
	ImGui::Begin("Tree IMGUI");

	if (ImGui::Button("Save Trees"))
	{
		if (SUCCEEDED(Save_Trees_JSON(m_szTreeSavePath)))
			strcpy_s(m_szTreeSavePath, sizeof(m_szTreeSavePath), "Save Success");
		else
			strcpy_s(m_szTreeSavePath, sizeof(m_szTreeSavePath), "Save Failed");
	}

	ImGui::SameLine();

	if (ImGui::Button("Load Trees"))
	{
		if (SUCCEEDED(Load_Trees_JSON(m_szTreeSavePath)))
			strcpy_s(m_szTreeSavePath, sizeof(m_szTreeSavePath), "Load Success");
		else
			strcpy_s(m_szTreeSavePath, sizeof(m_szTreeSavePath), "Load Failed");
	}


	ImGui::End();
	return S_OK;
}


unique_ptr<Tree> Tree::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Tree>(new Tree(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Tree");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Tree::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Tree(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Tree");
		return nullptr;
	}

	return pInstance;
}

HRESULT Tree::Add_Tree(const _float3& vPosition)
{
	return Add_Tree(vPosition,_float3{ 1.f, 1.f, 1.f },0.f);
}

HRESULT Tree::Add_Tree(const _float3& vPosition,const _float3& vScale,_float fYaw)
{
	if (m_pModelCom == nullptr)
		return E_FAIL;

	return m_pModelCom->Add_Instance(vPosition,vScale,fYaw);
}

HRESULT Tree::Erase_Tree(const _float3& vCenter,_float fRadius)
{
	if (m_pModelCom == nullptr)
		return E_FAIL;

	return m_pModelCom->Remove_Instance_InRadius(vCenter,fRadius);
}

HRESULT Tree::Clear_Trees()
{
	if (m_pModelCom == nullptr)
		return E_FAIL;

	return m_pModelCom->Clear_Instances();
}

uint32_t Tree::Get_TreeCount() const
{
	if (m_pModelCom == nullptr)
		return 0;

	return m_pModelCom->Get_InstanceCount();
}
void Tree::GUI_TreeBrush()
{
	ImGui::Begin("Tree Brush");

	ImGui::Checkbox("Tree Brush Mode", &m_bTreeBrushMode);

	if (ImGui::RadioButton("Paint", m_bEraseMode == false))
	{
		m_bEraseMode = false;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("Erase", m_bEraseMode == true))
	{
		m_bEraseMode = true;
	}

	ImGui::DragFloat("Brush Radius", &m_fBrushRadius, 0.1f, 0.1f, 100.f);
	ImGui::DragInt("Brush Count", &m_iBrushCount, 1, 1, 50);

	ImGui::DragFloat("Scale Min", &m_fScaleMin, 0.01f, 0.01f, 10.f);
	ImGui::DragFloat("Scale Max", &m_fScaleMax, 0.01f, 0.01f, 10.f);

	if (ImGui::Button("Clear Trees"))
	{
		Clear_Trees();
	}

	ImGui::Text("Tree Count : %d", Get_TreeCount());

	ImGui::End();
}
void Tree::Update_TreeBrush(_float fTimeDelta)
{
	if (m_bTreeBrushMode == false)
 		return;

	ImGuiIO& io = ImGui::GetIO();


	m_fBrushTimeAcc += fTimeDelta;

	_float4 vPickingPoint = {};

	// 여기서 Terrain Picking 필요
	// 네 엔진에 Terrain 객체 가져오는 방식에 맞춰서 수정해야 함
	
	auto pTerrain = dynamic_pointer_cast<Terrain>(
		CGameInstance::Get().Find_Object(
			CGameInstance::Get().Get_Level(),
			L"Base",
			L"Terrian"
		)
	);

	if (pTerrain == nullptr)
		return;


	CGameInstance::Get().Picking_to_Shader(&vPickingPoint);


	m_bBrushPicked = true;
	m_vBrushPickingPoint = vPickingPoint;

	// 좌클릭 : Paint
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (m_fBrushTimeAcc < m_fBrushInterval)
			return;

		m_fBrushTimeAcc = 0.f;

		if (m_bEraseMode == false)
		{
			for (int i = 0; i < m_iBrushCount; ++i)
			{
				_float4 vTreePos = vPickingPoint;

				float fRandomX = ((float)rand() / RAND_MAX) * 2.f - 1.f;
				float fRandomZ = ((float)rand() / RAND_MAX) * 2.f - 1.f;

				vTreePos.x += fRandomX * m_fBrushRadius;
				vTreePos.z += fRandomZ * m_fBrushRadius;

				float fScale =m_fScaleMin +((float)rand() / RAND_MAX) * (m_fScaleMax - m_fScaleMin);

				float fYaw =((float)rand() / RAND_MAX) * XM_2PI;

				Add_Tree(_float3{ vTreePos.x,  vTreePos.y , vTreePos.z }, _float3{ fScale, fScale, fScale }, fYaw);
			}
		}
		else
		{
			Erase_Tree(_float3{ vPickingPoint.x,  vPickingPoint.y , vPickingPoint.z }, m_fBrushRadius);
		}
	}

	// 우클릭은 강제 Erase로 써도 됨
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		if (m_fBrushTimeAcc < m_fBrushInterval)
			return;

		m_fBrushTimeAcc = 0.f;

		Erase_Tree(_float3{ vPickingPoint.x,  vPickingPoint.y , vPickingPoint.z }, m_fBrushRadius);
	}
}
HRESULT Tree::Save_Trees_JSON(const char* pFilePath)
{
	if (m_pModelCom == nullptr)
		return E_FAIL;

	return m_pModelCom->Save_Instances_JSON(pFilePath);
}

HRESULT Tree::Load_Trees_JSON(const char* pFilePath)
{
	if (m_pModelCom == nullptr)
		return E_FAIL;

	return m_pModelCom->Load_Instances_JSON(pFilePath);
}