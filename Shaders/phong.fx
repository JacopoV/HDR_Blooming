//--------------------------------------------------------------------------------------
// basic_lighting_effect.fx
//
// Copyright (c) Roberto Toldo 2010. Do not distribute. (Master Game Development - University of Verona)
//--------------------------------------------------------------------------------------

//Definizione variabili e strutture
matrix World;
matrix View;
matrix Projection;
float3 CameraPosition;
float4 DefaultColor;
bool  EnableLighting = true;

// Material
struct Material
{
    float4 Kd; // Coefficiente di riflessione diffusiva
    float4 Ks; // Coefficiente di riflessione speculare
    float4 Ka;  // Coefficiente di riflessione ambientale
    float4 Ke;  // Coefficiente di riflessione emissiva
    float Shininess;  // Esponente di shininess
};

// Proprietà Sorgente luminosa puntiforme
struct Light
{
    float4 Position; // Posizione luce
    float4 Color; // Colore della luce
    float4 Ambient;  // Valore del colore ambientale
    float3 Attenuation; // Coefficienti di attenuazione proporzionale alla distanza punto-luce (k0+k1*d+k2*d*d)
};


// Stato Sampling bilineare per texture
SamplerState samLinear
{
  
	Filter = MIN_MAG_MIP_LINEAR;  // bilineare
    AddressU = Wrap;
    AddressV = Wrap;
};

// Array contenente le luci della scena 
Light    g_lights[1];

// Materiale oggetto caricato
Material g_material;

//// Texture scena
//Texture2D g_texture;

// Strutture Input e Output per i Vertex shaders
struct VS_INPUT
{
    float4 pos          : POSITION;         //posizione
    float3 norm         : NORMAL;           //normale
	
};


// Nel vertex shader phong dobbiamo anche passare al rasterizer normali e posizioni in coordinate mondo
struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
    float3 norm: TEXCOORD0;
    float3 worldpos: TEXCOORD1;

};

// Stato Z-Buffer Abilitato
DepthStencilState EnableDepth{
	
	DepthEnable = true; 
	DepthWriteMask = ALL;  // Può essere ALL o ZERO
	DepthFunc = Less;  // Può essere Less Greater Less_Equal ...
	// Setup stencil states 
	StencilEnable = false; 

};

// Funzione per calcolare illuminazione da luci puntiformi con modello di phong
float4 CalcLighting( float3 worldNormal, float3 worldPos, float3 cameraDir, Material mat )
{
 	// TODO: Calcola valore di illuminazione di un punto 3d con coordinate worldPos, normali worldNorm, Camera direction cameradir e materiale mat, illuminato da due luci  g_lights[2].
	// colore di output
    float4 output = float4(0,0,0,0);
	int n_lights = 1;	// numero di luci

    for(int i=0; i<n_lights; i++)
    {
		
        float3 toLight = g_lights[i].Position.xyz - worldPos;
		
		// distanza della luce
        float lightDist = length( toLight );

		
        float fAtten = 1.0/dot( g_lights[i].Attenuation, float3(1,lightDist,lightDist*lightDist) );
		
		// normalizzo
        float3 lightDir = normalize( toLight );

		// Vettore halfangle
        float3 halfAngle = normalize( (cameraDir + lightDir) / 2 );
        
        // Componente diffusiva
        output += max(0,dot( lightDir, worldNormal ) * g_lights[i].Color * mat.Kd * fAtten ) ;
		// Componente ambientale
		output += g_lights[i].Ambient * mat.Ka;
        // Componente speculare
        output += max(0,pow( dot( halfAngle, worldNormal ), mat.Shininess ) * g_lights[i].Color * mat.Ks * fAtten );
        // Componente emissiva
        output += mat.Ke;
    }
        
    return output;
}





//--------------------------------------------------------------------------------------
// Vertex Shader - Phong shading: illuminazione per pixel
//--------------------------------------------------------------------------------------
VS_OUTPUT VSPhong( VS_INPUT input )
{
   	// TODO: Vertex SHader Phong shading
	VS_OUTPUT outputPhong = (VS_OUTPUT)0;  
    
    float4 worldPos = mul( input.pos, World );
    float4 cameraPos = mul( worldPos, View );
    outputPhong.pos = mul( cameraPos, Projection );

    float3 worldNormal = normalize( mul( input.norm, (float3x3)World ) );
    outputPhong.norm = worldNormal;
    outputPhong.worldpos = worldPos.xyz;
	outputPhong.color = DefaultColor; 
	
	return outputPhong;

}

//--------------------------------------------------------------------------------------
// Pixel Shader - Phong shading: illuminazione per pixel
//--------------------------------------------------------------------------------------
float4 PSPhong( VS_OUTPUT input ) : SV_Target
{
	// TODO: Pixel SHader Phong shading
	// Rinormalizza le normali
	input.norm = normalize(input.norm);
	
	// Calcola vettore punto-telecamera
    float3 camera = normalize(CameraPosition - input.worldpos.xyz);

	// se richiesta luce calcolo il valore corretto
	float4 color;
    if(EnableLighting){
		color = CalcLighting( input.norm, input.worldpos.xyz, camera, g_material );
		
    }
    else{
		color = input.color;
	} 


	return color;

}



//--------------------------------------------------------------------------------------
technique11 Phong
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSPhong() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSPhong() ) );  
        SetDepthStencilState(EnableDepth,0);
    }

	//pass P1
    //{
        //SetVertexShader( CompileShader( vs_4_0, VSQuad() ) );
        //SetGeometryShader( NULL );
        //SetPixelShader( CompileShader( ps_4_0, PSQuad() ) );  
        //SetDepthStencilState(EnableDepth,0);
    //}
}
