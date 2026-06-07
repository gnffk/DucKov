#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class Shader;
class VIBuffer_Terrain;
NS_END


NS_BEGIN(Client)

class Terrain final : public GameObject
{
private:
	Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Terrain(const Terrain& Prototype);
public:
	virtual ~Terrain();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<VIBuffer_Terrain>	m_pVIBufferCom = { nullptr };
	shared_ptr<Texture>				m_pGrassTex = { nullptr };
	shared_ptr<Texture>				m_pMudTex = { nullptr };
	shared_ptr<Texture>				m_pRoadTex = { nullptr };
	shared_ptr<Texture>				m_pSplatTex = { nullptr };

	shared_ptr<Shader>				m_pShaderCom = { nullptr };


#if _DEBUG
private:
	ComPtr<ID3D11Texture2D>          m_pSplatTexture2D;
	ComPtr<ID3D11ShaderResourceView> m_pSplatSRV;

	vector<_float4> m_SplatPixels;

	uint32_t m_iSplatWidth = 512;
	uint32_t m_iSplatHeight = 512;
	_float4  m_CheckPickTerrainNum{};

private:
	int   m_iPaintChannel = 0; // 0 Grass, 1 Mud
	float m_fBrushRadius = 1.f;
	float m_fBrushPower = 1.f;
	bool  m_bTerrainPaintMode = false;
private:
	void Save_SplatPNG(const char* pFilePath);
	_bool Load_SplatPNG(const char* pFilePath);
private:
	void GUI_TerrainPaint();
public:
	_bool Picking_Terrain_ForNavMesh(_float3& vPickingPoint);

public:

	HRESULT Create_SplatTexture();
	_bool Picking_Terrain(_float3& vPickingPoint);
	void Paint_Splat(_float3 vWorldPos);
	void Update_SplatTexture();
#endif
private:
	HRESULT Ready_Components();


public:

	static unique_ptr<Terrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END