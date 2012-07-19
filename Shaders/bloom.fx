//--------------------------------------------------------------------------------------
// blur.fx
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
Texture2D g_texture_phong;

// Strutture Input e Output per i Vertex shaders
struct VS_INPUT
{
    float4 pos          : POSITION;         //posizione
	float2 tex			: TEXCOORD;			//texture
};

struct VS_OUTPUT
{
    float4 pos          : SV_POSITION;		//posizione
	float2 tex			: TEXCOORD0;		//texture
};



//--------------------------------------------------------------------------------------
// Vertex Shader - tone mapping shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS_BLOOM( VS_INPUT input )
{
   	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = input.pos;
	output.tex = input.tex;

	return output;

}

//--------------------------------------------------------------------------------------
// Pixel Shader - tone mapping shader
//--------------------------------------------------------------------------------------
float4 PS_BLOOM( VS_OUTPUT input ) : SV_Target
{

	float4 color = float4(0,0,0,1);

	color = g_texture.Sample( samLinear, input.tex );  	
	
	float4 phong_color = g_texture_phong.Sample(samLinear,input.tex);

	color = lerp(color,phong_color,0.7);
	
	

	return color;
	

}


//--------------------------------------------------------------------------------------
technique11 Bloom
{

	pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_BLOOM() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_BLOOM() ) );
	}
	
}
