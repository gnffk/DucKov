#pragma once

#include "pch.h"


class Material
{
public:
	Material();

	~Material();



	HRESULT Set_Material(uint32_t _materialNum, vector<vector<TEXTUREINFO>> _textures);

	uint32_t m_materialNum;
	vector<vector<TEXTUREINFO>> m_textures;

};

