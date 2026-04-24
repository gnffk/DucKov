#pragma once

#include "pch.h"


class Material
{
public:
	Material();

	~Material();



	HRESULT Set_Material(uint32_t _materialtype, vector<TEXTUREINFO> _textures);

	uint32_t m_materialtype;
	uint32_t m_texturesSize;
	vector<TEXTUREINFO> m_textures;

};

