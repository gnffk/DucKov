#include "Engine_Macro.h"
#include "TestModel.h"
#include "GameInstance.h"

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
	if (nullptr != pArg) {
		auto		pDesc = static_cast<TestModelDec*>(pArg);
		m_iData = pDesc->iData;

		pDesc->pGameObjectTag = TEXT("TestModel");
		pDesc->fSpeedPerSec = 10.f;
		pDesc->fRotationPerSec = 180.f;


		if (FAILED(__super::Initialize(pDesc)))
			return E_FAIL;
	}
	else {
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}


	m_pViBuffer = static_pointer_cast<VIBuffer_Fbx>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), L"Prototype_Component_VIBuffer_Fbx_TestModel"));

	if (nullptr == m_pViBuffer) {
		return E_FAIL;
	}


	return S_OK;
}

void TestModel::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLEND, SHARED_THIS(TestModel));
}

void TestModel::Update(_float fTimeDelta)
{

}

void TestModel::Late_Update(_float fTimeDelta)
{
	
}

HRESULT TestModel::Render()
{

	//m_pViBuffer->Bind_Resources();

	//m_pViBuffer->Render();
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

void TestModel::CreateViewAndPerspective()
{
	//// Use DirectXMath to create view and perspective matrices.

	//DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.7f, 1.5f, 0.f);
	//DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, -0.1f, 0.0f, 0.f);
	//DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);
	//m_pContext->GetAspectRatio();
	//DirectX::XMStoreFloat4x4(
	//	pDevice.view,
	//	DirectX::XMMatrixTranspose(
	//		DirectX::XMMatrixLookAtRH(
	//			eye,
	//			at,
	//			up
	//		)
	//	)
	//);

	//float aspectRatioX = m_deviceResources->GetAspectRatio();
	//float aspectRatioY = aspectRatioX < (16.0f / 9.0f) ? aspectRatioX / (16.0f / 9.0f) : 1.0f;

	//DirectX::XMStoreFloat4x4(
	//	&m_constantBufferData.projection,
	//	DirectX::XMMatrixTranspose(
	//		DirectX::XMMatrixPerspectiveFovRH(
	//			2.0f * std::atan(std::tan(DirectX::XMConvertToRadians(70) * 0.5f) / aspectRatioY),
	//			aspectRatioX,
	//			0.01f,
	//			100.0f
	//		)
	//	)
	//);
}