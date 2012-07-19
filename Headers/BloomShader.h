#pragma once
#include "Shader.h"

class BloomShader : public Shader{
public:
	
	bool Initalize(ID3D11Device* pd3dDevice);
	void SetShaderTexturePhong(ID3D11ShaderResourceView* texture);

};