//--------------------------------------------------------------------------------------
// bloom.fx
//
// Copyright (c) Roberto Toldo 2010. Do not distribute. (Master Game Development - University of Verona)
//--------------------------------------------------------------------------------------

// Stato Sampling bilineare per texture
SamplerState samLinear
{
  
	Filter = MIN_MAG_MIP_LINEAR;  // bilineare
    AddressU = Wrap;
    AddressV = Wrap;
};

// Texture scena
Texture2D g_texture;


//// Strutture Input e Output per i Vertex shaders
struct VS_INPUT
{
    float4 pos          : POSITION;         //posizione
    float2 tex          : TEXCOORD;		//Coordinate texture
};


struct VS_OUTPUT
{
    float4 pos			: SV_POSITION;
	float2 tex			: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader - HighPaSS shading: aumento luminosità
//--------------------------------------------------------------------------------------
VS_OUTPUT VS_HIGH_PASS( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;  
    
	output.pos = input.pos;
	output.tex = input.tex; 
	
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader - HighPass shading: aumento luminsità 
//--------------------------------------------------------------------------------------
float4 PS_HIGH_PASS( VS_OUTPUT input ) : SV_Target
{

	float4 color = float4(0,0,0,0);
	color = g_texture.Sample(samLinear,input.tex);
	
	if( color.r < 0.566){ color.r = 0.1; }
	
	if( color.g < 0.566){ color.g = 0.1; }
	
	if( color.b < 0.566){ color.b = 0.1; }

	


	return color;

}

//--------------------------------------------------------------------------------------
technique11 Highpass
{
	pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_HIGH_PASS() ) );
		//SetVertexShader( NULL);
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_HIGH_PASS() ) );  
    }
}
