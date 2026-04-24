#pragma once
#include "Material.h"
#include "Shader.h"


Material::Material(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
}

Material::~Material()
{
}



HRESULT Material::Initialize(uint32_t materialtype,vector<TEXTUREINFO>& textures, const _char* ModelFilePath)
{
	_char	szDrive[MAX_PATH] = { };
	_char	szDir[MAX_PATH] = { };

	_splitpath_s(ModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
	int a = textures.size();

	for (auto texture : textures) {
		for (size_t j = 0; j < texture.m_textureNum; j++)
		{
			_char	szFileName[MAX_PATH] = { };
			_char	szExt[MAX_PATH] = { };
			_char	szFullPath[MAX_PATH] = {};

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			HRESULT         hr = {};
			ComPtr<ID3D11ShaderResourceView>		pSRV = { nullptr };

			_tchar	szFinalPath[MAX_PATH] = {};

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath),
				szFinalPath, MAX_PATH);



			if (false == strcmp(szExt, ".dds"))
				hr = CreateDDSTextureFromFile(m_pDevice.Get(), szFinalPath, nullptr, &pSRV);

			else if (false == strcmp(szExt, ".tga"))
				hr = E_FAIL;
			else
				hr = CreateWICTextureFromFile(m_pDevice.Get(), szFinalPath, nullptr, &pSRV);

			m_Materials[texture.m_textureType].emplace_back(pSRV);
		}
	}





	return S_OK;
}

HRESULT Material::Bind_ShaderResource(shared_ptr<class Shader> pShader, const _char* pConstantName, uint32_t eMaterialType, uint32_t iTextureIndex)
{
	return pShader->Bind_SRV(pConstantName,
		m_Materials[eMaterialType][iTextureIndex]);
}



shared_ptr<Material> Material::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t materialtype,vector<TEXTUREINFO>& textures, const _char* ModelFilePath)
{
	auto	pInstance = shared_ptr<Material>(new Material(pDevice, pContext));

	if (FAILED(pInstance->Initialize(materialtype, textures, ModelFilePath)))
		MSG_BOX("Failed to Created : Material");

	return pInstance;
}
