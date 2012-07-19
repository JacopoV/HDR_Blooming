//--------------------------------------------------------------------------------------
// File: BlurShaderV.h
// 
// **************************************
// Header classe per definizione risorsa Shader per effetto post processing -> Blur su asse verticale 
// **************************************
//
// Copyright (c) Volpin Jacopo 2010. Do not distribute. (Master Game Development - University of Verona)
//--------------------------------------------------------------------------------------


#pragma once
#include "Shader.h"

class BlurShaderV : public Shader{
public:
	
	bool Initalize(ID3D11Device* pd3dDevice);
	void SetBlurWindowSize(UINT height);

private:
	float blurSizeV;

};