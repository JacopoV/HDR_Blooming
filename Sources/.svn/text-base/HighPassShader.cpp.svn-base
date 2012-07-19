#include "../Headers/HighPassShader.h"


bool HighPassShader::Initalize(ID3D11Device* pd3dDevice)
{
	ID3D10Blob*	pBlob = NULL;
	ID3D10Blob*	pErrorBlob = NULL;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	#if defined( DEBUG ) || defined( _DEBUG )
	// Il settaggio di questo flag permette di eseguire il debug diretto dello shader, senza
	// impattare in modo marcato sulle performance.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	#endif
	// Creazione effetto in g_pEffect, caricandolo dal file fx
	HRESULT hr = D3DX11CompileFromFile(L"Shaders/highpass.fx", NULL, NULL,NULL, "fx_5_0", dwShaderFlags, 0,
		NULL, &pBlob, &pErrorBlob, NULL);


    if( FAILED( hr ) )
    {
		MessageBox(NULL, L"Problema nel caricamento di highpass.fx.", L"Error", MB_OK );

		if (pErrorBlob)
		{
			MessageBoxA(0, (char*)pErrorBlob->GetBufferPointer(), 0, 0);
		}
		return false;
    }

	D3DX11CreateEffectFromMemory(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pd3dDevice ,&m_Effect);

	// Trova la tecnica definita nel .fx - nota come un effetto.fx può avere diverse tecniche.
	m_Technique = m_Effect->GetTechniqueByName( "Highpass" );


	//Definiamo gli input da agganciare (bind) al vertex shader
	//In questo caso passeremo solo le informazioni sulla posizione con un vertex buffer
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = sizeof( layout ) / sizeof( layout[0] );

	// Creiamo e configuriamo il layout di input
	D3DX11_PASS_DESC PassDesc;
	m_Technique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	hr = pd3dDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_VertexLayout );
	if( FAILED( hr ) )
		return false;

	return true;
}

//void HighPassShader::Render(ID3D11DeviceContext* pd3dDeviceContext, UINT indexCount)
//{
//
//	D3DX11_TECHNIQUE_DESC techDesc;
//	// Ottieni la tecnica ed effettua il rendering
//	pd3dDeviceContext->IASetInputLayout(m_VertexLayout);
//
//	m_Technique->GetDesc( &techDesc );
//
//	pd3dDeviceContext->IASetInputLayout(m_VertexLayout);
//
//	for( UINT p = 0; p < techDesc.Passes; ++p )
//	{
//		m_Technique->GetPassByIndex( p )->Apply( 0, pd3dDeviceContext );
//		pd3dDeviceContext->DrawIndexed(indexCount, 0, 0 );
//	}
//}

