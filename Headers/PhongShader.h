//--------------------------------------------------------------------------------------
// File: PhongShader.h
// 
// **************************************
// Header classe per definizione risorsa Shader per effetto post processing -> illuminazione con Phong
// **************************************
//
// Copyright (c) Volpin Jacopo 2010. Do not distribute. (Master Game Development - University of Verona)
//--------------------------------------------------------------------------------------

#pragma once
#include "Shader.h"

class PhongShader : public Shader{

public:
	bool Initalize(ID3D11Device* pd3dDevice);
};