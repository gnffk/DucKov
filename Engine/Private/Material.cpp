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



HRESULT Material::Initialize( vector<vector<TEXTUREINFO>>& textureTypes, const _char* ModelFilePath)
{
	_char	szDrive[MAX_PATH] = { };
	_char	szDir[MAX_PATH] = { };

	_splitpath_s(ModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);


	for (auto textures : textureTypes) {
		for (auto texture : textures) {
			m_Materials[texture.m_textureType].reserve(textures.size());
			for (size_t j = 0; j < textures.size(); j++)
			{
				string File = texture.File;
				string Ext = texture.Ext;

				string fullPath = string(szDir);

		

				fullPath += File + Ext;

				wstring wPath(fullPath.begin(), fullPath.end());


				HRESULT         hr = {};
				ComPtr<ID3D11ShaderResourceView>		pSRV = { nullptr };


				if (Ext ==  ".dds")
					hr = CreateDDSTextureFromFile(m_pDevice.Get(), wPath.c_str(), nullptr, &pSRV);

				else if (Ext ==  ".tga")
					hr = E_FAIL;
				else
					hr = CreateWICTextureFromFile(m_pDevice.Get(), wPath.c_str(), nullptr, &pSRV);

				m_Materials[texture.m_textureType].emplace_back(pSRV);
			}
		}
	
	}





	return S_OK;
}

HRESULT Material::Bind_ShaderResource(shared_ptr<class Shader> pShader, const _char* pConstantName, uint32_t eMaterialType, uint32_t iTextureIndex)
{
	if (m_Materials[eMaterialType].size() == 0) {
		return S_OK;
	}

	return pShader->Bind_SRV(pConstantName,
		m_Materials[eMaterialType][iTextureIndex]);
}



shared_ptr<Material> Material::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,vector<vector<TEXTUREINFO>>& textureTypes, const _char* ModelFilePath)
{
	auto	pInstance = shared_ptr<Material>(new Material(pDevice, pContext));

	if (FAILED(pInstance->Initialize( textureTypes, ModelFilePath)))
		MSG_BOX("Failed to Created : Material");

	return pInstance;
}
