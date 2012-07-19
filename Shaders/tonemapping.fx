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
VS_OUTPUT VS_TONEMAPPING( VS_INPUT input )
{
   	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = input.pos;
	output.tex = input.tex;

	return output;

}

//--------------------------------------------------------------------------------------
// Pixel Shader - tone mapping shader
//--------------------------------------------------------------------------------------
float4 PS_TONEMAPPING( VS_OUTPUT input ) : SV_Target
{

	float4 color = float4(0,0,0,1);
	float2 pos_color_black = float2(0,0);
	float4 color_black = g_texture_phong.Sample(samLinear,input.tex);
	bool insert_black = false;

	// controllo se il pixel originario della scena iniziale era nero (texture di Phong)
	if(color_black.r == 0.0 && color_black.g == 0.0 && color_black.b == 0.0){
		pos_color_black = input.tex;
		insert_black = true;
	}

	// Parametri tonemapping
	float exposure = 0.75f;
	float whitePoint = 1.5f;

	
	color = g_texture.Sample( samLinear, input.tex );  	

	// Prendo i valori di luminanza 1x1 dalla texture

	float4 luminanceSample = g_texture.Sample( samLinear, float2( 0.5f, 0.5f ));  
  
	// Media della luminanza memorizzata in canale R

	float avgLuminance = luminanceSample.r;   

	// Conversione RGB -> XYZ 
	const float3x3 RGB2XYZ = {0.5141364, 0.3238786,  0.16036376, 
				  0.265068,  0.67023428, 0.06409157, 
				  0.0241188, 0.1228178,  0.84442666};	
				      

	float3 XYZ = mul(RGB2XYZ, color.rgb);    

	// conversione XYZ -> Yxy   

	float3 Yxy;  
	
	Yxy.r = XYZ.g;                            

	// copio luminanza Y  

	Yxy.g = XYZ.r / (XYZ.r + XYZ.g + XYZ.b ); 

	// x = X / (X + Y + Z)  

	Yxy.b = XYZ.g / (XYZ.r + XYZ.g + XYZ.b ); 

	// y = Y / (X + Y + Z)  
	// (Lp) mappa della media di luminanza in scala di grigi 

	float Lp = Yxy.r * exposure / (avgLuminance);
	
	// (Ld) scalo tutta la luminanza nel range tra 0 ed 1 

	Yxy.r = (Lp * (1.0f + Lp/(whitePoint * whitePoint)))/(1.0f + Lp);  

	// conversione Yxy -> XYZ   

	XYZ.r = Yxy.r * Yxy.g / Yxy. b;               

	// X = Y * x / y  

	XYZ.g = Yxy.r;                                

	// copio luminanza Y 
	
	XYZ.b = Yxy.r * (1 - Yxy.g - Yxy.b) / Yxy.b;  

	// Z = Y * (1-x-y) / y      
	
	// conversione XYZ -> RGB  

	const float3x3 XYZ2RGB  = { 2.5651,-1.1665,-0.3986,
			            -1.0217, 1.9777, 0.0439, 
  				    0.0753, -0.2543, 1.1892};  

	color.rgb = mul(XYZ2RGB, XYZ);  

	color.a = 1;

	float4 phong_color = g_texture_phong.Sample(samLinear,input.tex);

	color = lerp(color,phong_color,0.7);

	if(insert_black){

		color = g_texture.Sample(samLinear,pos_color_black);
		color.r -= 0.1f;
		color.g -= 0.1f;
		color.b -= 0.1f;

	}
	
	return color;
	

}


//--------------------------------------------------------------------------------------
technique11 ToneMapping
{

	pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_TONEMAPPING() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_TONEMAPPING() ) );
	}
	
}
