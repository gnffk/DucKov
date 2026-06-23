#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


NS_BEGIN(Engine)

class Model;
class Shader;
class BaseCollider;
NS_END

NS_BEGIN(Client)
// 
class Bullet final : public GameObject
{

public:
	typedef struct tagBulletDesc : public GameObject::GAMEOBJECT_DESC
	{
		_float3 vStartPos = {};
		_float3 vDir = { 0.f, 0.f, 1.f };
		_float  fSpeed = 30.f;
	} BULLET_DESC;

	struct TrailPoint
	{
		_float3 vPos;
		float   fLife;
	};


private:
	_float3 m_vDir = { 0.f, 0.f, 1.f };
	_float  m_fSpeed = 30.f;
private:
	Bullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Bullet(const Bullet& Prototype);
public:
	virtual ~Bullet();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Ready_Components();

private:
	HRESULT Ready_Trail();
	void Build_TrailMesh();
	HRESULT Render_Trail();

private:
	vector<TrailPoint> m_TrailPoints;
	vector<VTXBULLETTRAIL>  m_TrailVertices;

	ComPtr<ID3D11Buffer> m_pTrailVB = nullptr;

	shared_ptr<Shader> m_pTrailShaderCom = nullptr;
	shared_ptr<Texture> m_pTrailTextureCom = nullptr;

	float m_fTrailLife = 0.15f;
	float m_fTrailWidth = 0.3f;

private:
	_float m_fLifeTime = 5.f;
	_float m_fLifeTimer = 0.f;
private:
	static constexpr uint32_t MAX_TRAIL_POINTS = 32;
	static constexpr uint32_t MAX_TRAIL_VERTICES = MAX_TRAIL_POINTS * 2;
private:
	uint32_t			m_iData = {};

	_wstring			m_ModelComponentName;
private:

	shared_ptr<Model>			m_pModelCom = { nullptr };
	shared_ptr<Shader>			m_pShaderCom = { nullptr };




public:
	static unique_ptr<Bullet> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;


};

NS_END