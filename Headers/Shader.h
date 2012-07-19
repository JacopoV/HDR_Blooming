//--------------------------------------------------------------------------------------
// File: Shader.h
// 
// **************************************
// Header classe per definizione risorsa Shader per effetto post processing , classe base
// **************************************
//
// Copyright (c) Volpin Jacopo 2010. Do not distribute. (Master Game Development - University of Verona)
//--------------------------------------------------------------------------------------


#pragma once
#include "../DXUT/DXUT.h"
#include "../Effects11/d3dx11effect.h"

class Shader{
public:
	Shader();
	bool Initalize(ID3D11Device* pd3dDevice);
	void SetShaderTexture(ID3D11ShaderResourceView*);

	void Render(ID3D11DeviceContext*,UINT);

	ID3DX11Effect* GetEffect();
	ID3DX11EffectTechnique* GetTechnique();
	ID3D11InputLayout* GetVertexLayout();

	~Shader();

protected:
	ID3DX11Effect* m_Effect;
	ID3DX11EffectTechnique*  m_Technique;
	ID3D11InputLayout* m_VertexLayout;
};