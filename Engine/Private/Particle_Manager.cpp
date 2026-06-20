#include "Particle_Manager.h"


Particle_Manager::Particle_Manager(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
}

Particle_Manager::~Particle_Manager()
{
	Clear();
}

HRESULT Particle_Manager::Register_ParticleSystem(PARTICLE_TYPE eType,shared_ptr<Particle_System> pParticleSystem)
{
	if (pParticleSystem == nullptr)
		return E_FAIL;

	m_ParticleSystems[eType] = pParticleSystem;

	return S_OK;
}

HRESULT Particle_Manager::Add_Particle(PARTICLE_TYPE eType, void* pArg)
{

	switch (eType) {
	case PARTICLE_TYPE::BLOOD:
		{
			auto pParicle = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(),L"Effect", L"Particle_Blood");
			if(pParicle == nullptr)
			{

				GameObject::GAMEOBJECT_DESC Desc{};

				Desc.ObjectType = ETOUI(OBJECTTYPE::OBJECT_STATIC);
				Desc.m_strName = L"Particle_Blood";
				Desc.m_strPrototypeObjectName = L"Prototype_GameObject_Particle_Blood";
				Desc.m_strPrototypeBaseName = L"Particle_Blood";
				Desc.fSpeedPerSec = 0.f;
				Desc.fRotationPerSec = 0.f;

				if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Particle_Blood"), CGameInstance::Get().Get_Level(), TEXT("Effect"), &Desc)))
				{
					return E_FAIL;
				}

				auto pBlood = dynamic_pointer_cast<Particle_System>(CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), TEXT("Effect"), TEXT("Particle_Blood")));

				if (pBlood == nullptr)
					return E_FAIL;

				if (FAILED(CGameInstance::Get().Register_ParticleSystem(PARTICLE_TYPE::BLOOD, pBlood)))
				{
					return E_FAIL;
				}
			}

		}
		break;
	}

	auto        pDesc = static_cast<Particle_System::PARTICLE_SPAWN_DESC*>(pArg);


	auto iter = m_ParticleSystems.find(eType);

	if (iter == m_ParticleSystems.end())
		return E_FAIL;

	auto pParticleSystem = iter->second.lock();

	if (pParticleSystem == nullptr)
		return E_FAIL;

	pParticleSystem->Add_Particle(*pDesc);

	return S_OK;
}


void Particle_Manager::Clear()
{
	m_ParticleSystems.clear();
}

unique_ptr<Particle_Manager> Particle_Manager::Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext)
{
	return unique_ptr<Particle_Manager>(new Particle_Manager(pDevice, pContext));
}

