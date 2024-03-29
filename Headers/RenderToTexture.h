//--------------------------------------------------------------------------------------
// File: RenderToTexture.h
// 
// **************************************
// Header classe per definizione gestore texture. Riferisce le texture per uno shader specifico e le aggiorna.
// Renderizza su quadrato il risultato finale dello shader.
// **************************************
//
// Copyright (c) Volpin Jacopo 2010. Do not distribute. (Master Game Development - University of Verona)
//--------------------------------------------------------------------------------------


#pragma once
#include "../Effects11/d3dx11effect.h"
#include "../DXUT/DXUT.h"
class RenderToTexture
{
public:
	RenderToTexture();
    ~RenderToTexture();

	bool Initialize(ID3D11Device*, int, int);

	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float*);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
};
