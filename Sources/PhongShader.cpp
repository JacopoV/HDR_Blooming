#include "../Headers/PhongShader.h"

bool PhongShader::Initalize(ID3D11Device* pd3dDevice)
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
	HRESULT hr = D3DX11CompileFromFile(L"Shaders/phong.fx", NULL, NULL,NULL, "fx_5_0", dwShaderFlags, 0,
		NULL, &pBlob, &pErrorBlob, NULL);


    if( FAILED( hr ) )
    {
		MessageBox(NULL, L"Problema nel caricamento di phong.fx.", L"Error", MB_OK );

		if (pErrorBlob)
		{
			MessageBoxA(0, (char*)pErrorBlob->GetBufferPointer(), 0, 0);
		}
		return false;
    }

	D3DX11CreateEffectFromMemory(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pd3dDevice ,&m_Effect);

	// Trova la tecnica definita nel .fx - nota come un effetto.fx può avere diverse tecniche.
	m_Technique = m_Effect->GetTechniqueByName( "Phong" );


	//Definiamo gli input da agganciare (bind) al vertex shader
	//In questo caso passeremo solo le informazioni sulla posizione con un vertex buffer
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
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

