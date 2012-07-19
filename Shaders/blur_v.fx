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

// Blur parameter
float blurSizeV;



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
// Vertex Shader - blur shading
//--------------------------------------------------------------------------------------
VS_OUTPUT VS_BLUR( VS_INPUT input )
{
   	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = input.pos;
	output.tex = input.tex;

	return output;

}

//--------------------------------------------------------------------------------------
// Pixel Shader - horizontal blur shading
//--------------------------------------------------------------------------------------
float4 PS_BLUR( VS_OUTPUT input ) : SV_Target
{

	float4 color = float4(0,0,0,0);
	
	// blur (vertical)
	color += g_texture.Sample(samLinear, float2(input.tex.x , input.tex.y - 4.0*blurSizeV)) * 0.05;
    color += g_texture.Sample(samLinear, float2(input.tex.x, input.tex.y  - 3.0*blurSizeV)) * 0.09;
    color += g_texture.Sample(samLinear, float2(input.tex.x , input.tex.y - 2.0*blurSizeV)) * 0.12;
    color += g_texture.Sample(samLinear, float2(input.tex.x , input.tex.y - blurSizeV)) * 0.15;
    color += g_texture.Sample(samLinear, float2(input.tex.x, input.tex.y)) * 0.16;
    color += g_texture.Sample(samLinear, float2(input.tex.x , input.tex.y + blurSizeV)) * 0.15;
    color += g_texture.Sample(samLinear, float2(input.tex.x , input.tex.y + 2.0*blurSizeV)) * 0.12;
    color += g_texture.Sample(samLinear, float2(input.tex.x , input.tex.y + 3.0*blurSizeV)) * 0.09;
    color += g_texture.Sample(samLinear, float2(input.tex.x , input.tex.y + 4.0*blurSizeV)) * 0.05;
	
	
	return color;

}


//--------------------------------------------------------------------------------------
technique11 Blur_v
{

	pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_BLUR() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_BLUR() ) );
	}
	
}
