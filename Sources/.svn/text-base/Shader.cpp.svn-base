#include "../Headers/Shader.h"

Shader::Shader()
{
	m_Effect = NULL;
	m_Technique = NULL;
	m_VertexLayout = NULL;
}


void Shader::Render(ID3D11DeviceContext* pd3dDeviceContext, UINT indexCount)
{

	D3DX11_TECHNIQUE_DESC techDesc;
	// Ottieni la tecnica ed effettua il rendering
	pd3dDeviceContext->IASetInputLayout(m_VertexLayout);

	m_Technique->GetDesc( &techDesc );

	pd3dDeviceContext->IASetInputLayout(m_VertexLayout);

	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_Technique->GetPassByIndex( p )->Apply( 0, pd3dDeviceContext );
		pd3dDeviceContext->DrawIndexed(indexCount, 0, 0 );
	}
}

Shader::~Shader(){

	
	if(m_VertexLayout)
	{
		m_VertexLayout->Release();
		m_VertexLayout = NULL;
	}

	m_Technique = NULL;

	if(m_Effect)
	{
		m_Effect->Release();
		m_Effect = NULL;
	}
}

void Shader::SetShaderTexture(ID3D11ShaderResourceView* texture)
{
	m_Effect->GetVariableByName("g_texture")->AsShaderResource()->SetResource(texture);
}

ID3DX11Effect* Shader::GetEffect()
{
	return m_Effect;
	
}

ID3DX11EffectTechnique* Shader::GetTechnique()
{
	return m_Technique;
}

ID3D11InputLayout* Shader::GetVertexLayout()
{
	return m_VertexLayout;
}


