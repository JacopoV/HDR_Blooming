//--------------------------------------------------------------------------------------
// File: HDR_BLOOM.cpp
// 
// **************************************
// Illuminazione - Illuminazione modelli di Phong-Blind con Shading di Phong
// Applicazione filtri post processing : HighPass - Blur - ToneMapping
// Simulazione di illuminazione HDR con effetto bloom.
// **************************************
//
// Copyright (c) Volpin Jacopo 2010. Do not distribute. (Master Game Development - University of Verona)
//--------------------------------------------------------------------------------------


#include "DXUT/DXUT.h"
#include "DXUT/DXUTmisc.h"
#include "DXUT/DXUTgui.h"
#include "DXUT/SDKmisc.h"
#include "DXUT/SDKmesh.h"
#include "Effects11/d3dx11effect.h"
#include "Headers/CameraManager.h"
#include "Headers/RenderToTexture.h"
#include "Headers/HighPassShader.h"
#include "Headers/BlurShaderH.h"
#include "Headers/BlurShaderV.h"
#include "Headers/PhongShader.h"
#include "Headers/ToneMappingShader.h"

#define DEG2RAD( a ) ( a * D3DX_PI / 180.f )

//--------------------------------------------------------------------------------------
// Dichiarazione strutture
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
};


// Struttura che definisce un materiale associato
struct sMaterial{
	D3DXVECTOR4 mKe; // Coefficiente di riflessione emissiva
	D3DXVECTOR4 mKa; // Coefficiente di riflessione ambientale
	D3DXVECTOR4 mKd; // Coefficiente di riflessione diffusiva
	D3DXVECTOR4 mKs; // Coefficiente di riflessione speculare
	float mShininess; // esponente shininess
};
// Struttura che definisce una luce. Definisce anche una mesh per disegnare la luce.
struct sPointLight{
	// Mesh
	D3DXVECTOR4 mColour; // Colore luce
	D3DXVECTOR4 mAmbientColour; // Colore ambientale
	D3DXVECTOR3 mAttenuation; // Coefficienti di attenuazione proporzionale alla distanza punto-luce (k0+k1*d+k2*d*d)
};
//--------------------------------------------------------------------------------------
// Dichiarazione variabili globali
//--------------------------------------------------------------------------------------
CDXUTTextHelper *gmTextHelper = NULL;				 // Text Helper Class provided by DXUT
bool gbToggleFps = false;							 // Fps Text Switch
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs (included text)
bool gbToggleLights = false;
bool gbToggleCommands = false;						 // flag per switch camera / luce (false == camera, true == light) 


// Definiamo due mesh, teapot e flat
CDXUTSDKMesh g_MeshTeapot; // Mesh geometrica - dovrà contenenre vertici e normali (buffer)

// Definiamo anche i due materiali
sMaterial g_MaterialTeapot;
// Definiamo le matrici mondo relative alle due mesh
D3DXMATRIX  g_WorldTeapot; // Posizione orientazione nel mondo


// Definiamo luci puntuali
sPointLight g_Light1;
float g_LightX = 5.0f;
float g_LightZ = 4.0f;
float upperLightLimit = 2.0f;
float lowerLightLimit = 1.0f;
CDXUTSDKMesh g_MeshSphere; // Mesh - per disegnare il punto della luce
D3DXMATRIX  g_WorldLight1; // Posizione orientazione nel mondo luce 1


ID3D11Buffer      *pVBuffer;    // global

// Camera
CameraManager     *camera = NULL;

// Puntatori risorte shader
PhongShader       *phongShader = NULL;
HighPassShader    *highPassShader = NULL;
BlurShaderH       *blurShaderH = NULL; 
BlurShaderV       *blurShaderV = NULL;
ToneMappingShader *toneMappingShader = NULL;

// RenderToTexture

RenderToTexture   *texturePhong = NULL;
RenderToTexture   *textureHighPass = NULL;
RenderToTexture   *textureBlurH = NULL;
RenderToTexture   *textureBlurV = NULL;
RenderToTexture   *textureToneMapping = NULL;


//--------------------------------------------------------------------------------------
// Dichiarazione prototipi funzione 
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);

HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
                                      void* pUserContext );
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext);
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D11DestroyDevice( void* pUserContext );

LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

void PrintTitleAndFps();
void SetMaterial(sMaterial &MeshMaterial);
void SetupLights();

//--------------------------------------------------------------------------------------
// Funzione main del programma. Inizializza la finestra utilizzando DXUT ed entra nel main loop.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Settiamo le callback di DXUT
    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );

    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

    DXUTInit( true, // Effettua il parsing della linea di comando
		true, // mostra errori come message box
		NULL ); // nessun comando extra alla linea di comando
    DXUTSetCursorSettings( true, true ); // Mostra il cursore e nascondilo quando in full screen
	DXUTCreateWindow( L"MGD - HDR Lighting with Bloom " );
    DXUTCreateDevice( D3D_FEATURE_LEVEL_9_1, true, 640, 480 );
    DXUTMainLoop(); // entra nel main loop.

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Controlla i device disponibili e Rifiuta ogni device non compabile.
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext){
    return true;
}



//--------------------------------------------------------------------------------------
// Inizializzazione shaders e renderToTextures 
//--------------------------------------------------------------------------------------
void SetUpShader_RenderToTexture(ID3D11Device* pd3dDevice){

	UINT width = DXUTGetWindowWidth();
	UINT height = DXUTGetWindowHeight();

	// inizializzazione PhongShader
	phongShader = new PhongShader();
	phongShader->Initalize(pd3dDevice);

	// inizializzazione HihgPassShader
	highPassShader = new HighPassShader();
	highPassShader->Initalize(pd3dDevice);

	// inizializzazione BlurShader
	blurShaderH = new BlurShaderH();
	blurShaderH->Initalize(pd3dDevice);

	blurShaderV = new BlurShaderV();
	blurShaderV->Initalize(pd3dDevice);

	// inizializzazione ToneMappingShader
	toneMappingShader = new ToneMappingShader();
	toneMappingShader->Initalize(pd3dDevice);

	// inizializzazione RenderToTexture
	texturePhong = new RenderToTexture();
	texturePhong->Initialize(pd3dDevice,width,height);

	textureHighPass = new RenderToTexture();
	textureHighPass->Initialize(pd3dDevice,width,height);
	
	textureBlurH = new RenderToTexture();
	textureBlurH->Initialize(pd3dDevice,width,height);
	
	textureBlurV = new RenderToTexture();
	textureBlurV->Initialize(pd3dDevice,width,height);
	
	textureToneMapping = new RenderToTexture();
	textureToneMapping->Initialize(pd3dDevice,width,height);

};



//--------------------------------------------------------------------------------------
// Gestione descrizione texture e parametri render to texture 
//--------------------------------------------------------------------------------------
void ManageDescTexture(ID3D11Device* pd3dDevice){

	// in caso di resize della finestra, devo reimpostare le texture di supporto
	// per gli effetti di post processing in relazione alla nuove dimensioni finestra

	UINT width = DXUTGetWindowWidth();
	UINT height = DXUTGetWindowHeight();

	// distruzione delle vecchie texture ed inizializzazione delle nuove
	if(texturePhong){ delete texturePhong; texturePhong = new RenderToTexture();
		              texturePhong->Initialize(pd3dDevice,width,height); }
	if(textureHighPass){ delete textureHighPass; textureHighPass = new RenderToTexture();
		              textureHighPass->Initialize(pd3dDevice,width,height); }
	if(textureBlurH){ delete textureBlurH; textureBlurH = new RenderToTexture();
		              textureBlurH->Initialize(pd3dDevice,width,height); }
	if(textureBlurV){ delete textureBlurV; textureBlurV = new RenderToTexture();
		              textureBlurV->Initialize(pd3dDevice,width,height); }
	if(textureToneMapping){ delete textureToneMapping; textureToneMapping = new RenderToTexture();
		              textureToneMapping->Initialize(pd3dDevice,width,height); }

}


//--------------------------------------------------------------------------------------
// Inizializza le risorse da utilizzare. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
                                      void* pUserContext )
{

	// inizializzazione Camera
	camera = new CameraManager();
	camera->Initalize(D3DXVECTOR3( 0.0f, 3.0f, -10.0f ), D3DXVECTOR3( 0.0f, 1.0f, 0.0f ),D3DXVECTOR3( 0.0f, 0.0f, 0.0f ));

	// inizializzazione shader e renderToTexture
	SetUpShader_RenderToTexture(pd3dDevice);


	ID3D10Blob*	pBlob = NULL;
	ID3D10Blob*	pErrorBlob = NULL;

	g_DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, DXUTGetD3D11DeviceContext());

	gmTextHelper = new CDXUTTextHelper( pd3dDevice, DXUTGetD3D11DeviceContext(),&g_DialogResourceManager,15 );

	gbToggleFps = false;
	
	// Attiva le luci di default
	gbToggleLights = true;

	// definizione vertici per il quad da renderizzare a schermo intero
	SimpleVertex vertices[] =
	{   
		{D3DXVECTOR3(1.0f, -1.0f, 0.0f), D3DXVECTOR2(1, 1)},
		{D3DXVECTOR3(-1.0f, -1.0f, 0.0f), D3DXVECTOR2(0, 1)},
		{D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DXVECTOR2(0,0)},

		{D3DXVECTOR3(1.0f, -1.0f, 0.0f), D3DXVECTOR2(1,1)},
		{D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DXVECTOR2(0,0)},
		{D3DXVECTOR3(1.0f, 1.0f, 0.0f), D3DXVECTOR2(1,0)},
    };


	D3D11_BUFFER_DESC bd;
    bd.Usage = D3D11_USAGE_DEFAULT; 
    bd.ByteWidth = sizeof( SimpleVertex ) * 6; // Dimensione
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;  // Binding all’INPUTASSEMBLER
    bd.CPUAccessFlags = 0; // La cpu non ha accesso in scrittura.
    bd.MiscFlags = 0; // nessun flag addizionale

	D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices;

    HRESULT hr = pd3dDevice->CreateBuffer( &bd, &InitData, &pVBuffer );

	 if( FAILED( hr ) )
        return hr;
	
	D3DXMATRIX translation;
	D3DXMATRIX scale;

	// carico mesh teapot e sfera
	g_MeshTeapot.Create(pd3dDevice,L"/Meshes/teapot.sdkmesh");
	g_MeshSphere.Create(pd3dDevice,L"/Meshes/ball.sdkmesh");


	D3DXMatrixIdentity(&g_WorldTeapot);
	D3DXMatrixScaling(&scale,2.0f,2.0f,2.0f);
	D3DXMatrixTranslation(&translation,0.0f,1.0f,0.0f); // sollevata rispetto il flat
	D3DXMatrixMultiply(&g_WorldTeapot, &scale, &translation);


	// definizione parametri materiale teapot
	g_MaterialTeapot.mKe = D3DXVECTOR4(0,0,0,1.0f);
	g_MaterialTeapot.mKa = D3DXVECTOR4(0.02f,0.02f,0.02f,1.0f);
	g_MaterialTeapot.mKd = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f);
	g_MaterialTeapot.mKs = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f);
	g_MaterialTeapot.mShininess = 64;
	
	
	// definizione parametri relaviti alla luce
	g_Light1.mAmbientColour = D3DXVECTOR4(0.8f,0.8f,0.8f,1.0f);  
	g_Light1.mColour = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f);
	g_Light1.mAttenuation = D3DXVECTOR3(1.0f,0.0f,0.0f);  

	D3DXMatrixIdentity(&g_WorldLight1);
	D3DXMatrixScaling(&scale,0.5f,0.5f,0.5f);
	D3DXMatrixTranslation(&translation,5.0f,5.0f,4.0f);
	D3DXMatrixMultiply(&g_WorldLight1, &scale, &translation);

	
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Crea ogni risorsa dipendendente dal back buffer.
// Resetta l'aspectratio modificando viewport o matrice di proiezione.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext ){

    ManageDescTexture(pd3dDevice);
	g_DialogResourceManager.OnD3D11ResizedSwapChain( pd3dDevice, pBufferSurfaceDesc );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Rendering della geometria
//--------------------------------------------------------------------------------------
void DrawGeometry (ID3D11DeviceContext* pd3dImmediateContext){

	// setting della texture nello shader
	
	pd3dImmediateContext->IASetInputLayout( phongShader->GetVertexLayout());

	
	// Aggiorna le matrici di Viewing (telecamera) e projection
	phongShader->GetEffect()->GetVariableByName("View")->AsMatrix()->SetMatrix( ( float* )&camera->GetViewMatrix() );
	phongShader->GetEffect()->GetVariableByName("Projection")->AsMatrix()->SetMatrix( ( float* )&camera->GetProjectionMatrix() );
	phongShader->GetEffect()->GetVariableByName("CameraPosition")->AsVector()->SetFloatVector( ( float* )camera->GetViewEyeVector());

	
	// Abilita le luci
	phongShader->GetEffect()->GetVariableByName("EnableLighting")->AsScalar()->SetBool(gbToggleLights);
	phongShader->GetEffect()->GetVariableByName("DefaultColor")->AsVector()->SetFloatVector( ( float* )D3DXVECTOR4(1.0f,1.00f,0.05f,1.0) );
	SetupLights();
	
	
	// Setta material teapot
	SetMaterial(g_MaterialTeapot);

	phongShader->GetEffect()->GetVariableByName("World")->AsMatrix()->SetMatrix(g_WorldTeapot);
	phongShader->GetTechnique()->GetPassByIndex(0)->Apply( 0, pd3dImmediateContext );
	g_MeshTeapot.Render(pd3dImmediateContext);

	// rendering luci

	phongShader->GetEffect()->GetVariableByName("EnableLighting")->AsScalar()->SetBool(false);

	
	phongShader->GetEffect()->GetVariableByName("World")->AsMatrix()->SetMatrix(g_WorldLight1);
	phongShader->GetEffect()->GetVariableByName("DefaultColor")->AsVector()->SetFloatVector( ( float* )g_Light1.mColour );
	
	
	phongShader->GetTechnique()->GetPassByIndex(0)->Apply( 0, pd3dImmediateContext );
	g_MeshSphere.Render(pd3dImmediateContext);

}



//--------------------------------------------------------------------------------------
// Effettua il rendering
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext){

	 ID3D11RenderTargetView* g_rtv = NULL;
	 ID3D11DepthStencilView* g_dsv = NULL;

	 UINT pStride = sizeof(SimpleVertex);
	 UINT offset = 0;

	  // Pulisci back buffer
     float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // red, green, blue, alpha

	 // prendo i render target
	 pd3dImmediateContext->OMGetRenderTargets(1, &g_rtv, &g_dsv);

	
	 // setto i render target della texture per applicare il Phong Shader
	 texturePhong->SetRenderTarget(pd3dImmediateContext,g_dsv);
	 texturePhong->ClearRenderTarget(pd3dImmediateContext,g_dsv,ClearColor);

	 // renderizzo a texture la scena
	 DrawGeometry(pd3dImmediateContext);

	 pd3dImmediateContext->IASetVertexBuffers(0,1,&pVBuffer,&pStride,&offset);


	 // **** applico passo : HighPass Shader ****

	 highPassShader->SetShaderTexture(texturePhong->GetShaderResourceView());

	 textureHighPass->SetRenderTarget(pd3dImmediateContext,g_dsv);
	 textureHighPass->ClearRenderTarget(pd3dImmediateContext,g_dsv,ClearColor);

	 highPassShader->Render(pd3dImmediateContext,6);

	 // **** termine passo : HighPass Shader ****


	 // **** applico passo : Blur Shader Horizontal ****
	 
	 blurShaderH->SetShaderTexture(textureHighPass->GetShaderResourceView());

	 textureBlurH->SetRenderTarget(pd3dImmediateContext,g_dsv);
	 textureBlurH->ClearRenderTarget(pd3dImmediateContext,g_dsv,ClearColor);

	 blurShaderH->Render(pd3dImmediateContext,6);
	 
	 // **** termine passo : Blur Shader Horizontal ****

	 // **** applico passo : Blur Shader Vertical ****
	 
	 blurShaderV->SetShaderTexture(textureBlurH->GetShaderResourceView());

	 textureBlurV->SetRenderTarget(pd3dImmediateContext,g_dsv);
	 textureBlurV->ClearRenderTarget(pd3dImmediateContext,g_dsv,ClearColor);

	 blurShaderV->Render(pd3dImmediateContext,6);
	 
	 // **** termine passo : Blur Shader Vertical ****


	 // **** applico passo : ToneMapping Shader ****

	 toneMappingShader->SetShaderTexture(textureBlurV->GetShaderResourceView());
	 toneMappingShader->SetShaderTexturePhong(texturePhong->GetShaderResourceView());
	 
	 pd3dImmediateContext->OMSetRenderTargets(1, &g_rtv, g_dsv);
	 pd3dImmediateContext->ClearDepthStencilView( g_dsv, D3D11_CLEAR_DEPTH, 1.0, 0 );

	 toneMappingShader->Render(pd3dImmediateContext,6);

	 // **** termine passo : ToneMapping Shader ****

	 if(g_rtv) g_rtv->Release();
	 if(g_dsv) g_dsv->Release();

	 // Unbind delle risorse shader
     ID3D11ShaderResourceView* nullViews[8] = {0, 0, 0, 0, 0, 0, 0, 0};
     pd3dImmediateContext->PSSetShaderResources(0, 8, nullViews);

	
	 PrintTitleAndFps();
}

//--------------------------------------------------------------------------------------
// Stampa nella parte alta dello schermo titolo e frame per secondo
//--------------------------------------------------------------------------------------
void PrintTitleAndFps(){

	gmTextHelper->Begin();
	gmTextHelper->SetInsertionPos( 1, 1 );
	gmTextHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );		

	// Stampa la stringa contenente i FPS
	if(gbToggleFps){
		gmTextHelper->DrawTextLine(DXUTGetFrameStats(true));
	}

	gmTextHelper->DrawTextLine(L"Commands:");
	gmTextHelper->DrawTextLine(L"1 - increase light luminance");
	gmTextHelper->DrawTextLine(L"2 - decrease light luminance");
	gmTextHelper->DrawTextLine(L"C - switch camera/light commands");
	
	gmTextHelper->SetInsertionPos( 10, DXUTGetWindowHeight()-50);
	gmTextHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	gmTextHelper->DrawTextLine(L"light luminance:");

	WCHAR sz[5];
	swprintf_s(sz, L"%2.1f", g_Light1.mColour.x );

	gmTextHelper->DrawTextLine(sz);

	gmTextHelper->SetInsertionPos( DXUTGetWindowWidth()-150, DXUTGetWindowHeight()-50);
	gmTextHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	gmTextHelper->DrawTextLine(L"Active commands on:");
	gmTextHelper->SetInsertionPos( DXUTGetWindowWidth()-55, DXUTGetWindowHeight()-34);
	if(!gbToggleCommands){
		gmTextHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
		gmTextHelper->DrawTextLine(L"CAMERA");
	}
	else{
		gmTextHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
		gmTextHelper->DrawTextLine(L"LIGHT");
	}





	gmTextHelper->End();
}

//--------------------------------------------------------------------------------------
// Rilascia eventuali risorse create in OnD3D11ResizedSwapChain
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
	g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}

//--------------------------------------------------------------------------------------
// Rilascia eventuali risorse create in OnD3D11CreateDevice 
// Device-Swapbuffer e rendertarget sono rilasciate in automatico.
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
	if(gmTextHelper)
		delete gmTextHelper;
	gmTextHelper = 0;

	
	g_DialogResourceManager.OnD3D11DestroyDevice();
	

	// rimozione camera
	if( camera ) { delete camera; camera = NULL;}

	// rimozione buffer e risorse shader di supporto
	if ( pVBuffer) { pVBuffer->Release(); pVBuffer = NULL;}
	if( phongShader ) { delete phongShader; }
	if( highPassShader ) { delete highPassShader; highPassShader = NULL; }
	if( blurShaderH )  { delete blurShaderH; blurShaderH = NULL; }
	if( blurShaderV )  { delete blurShaderV; blurShaderV = NULL; }
	if( toneMappingShader ) { delete toneMappingShader; toneMappingShader = NULL; }


	// rimozione texture di supporto
	if( textureBlurV ) { delete textureBlurV; textureBlurV = NULL; }
	if( textureBlurH ) { delete textureBlurH; textureBlurH = NULL; }
	if( texturePhong ) { delete texturePhong; texturePhong = NULL; }
	if( textureHighPass ) { delete textureHighPass; textureHighPass = NULL;}
	if( textureToneMapping ) { delete textureToneMapping; textureToneMapping = NULL; }

	// rimozione e distruzione mesh
	g_MeshTeapot.Destroy();
	g_MeshSphere.Destroy();

	

}

//--------------------------------------------------------------------------------------
// Chiamata prima di inizializzare un device, ci permette di specificare parametri personalizzati per il device
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	pDeviceSettings->d3d11.AutoCreateDepthStencil = true;

    return true;
}

//--------------------------------------------------------------------------------------
// Gestisce modifiche alla scena.  Le animazioni andranno inserite qui. Chiamata prima di OnD3D11FrameRender nel main loop
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{

	D3DXMATRIX rotation;
	D3DXMATRIX translation;
	D3DXMATRIX scale;

	// Gestione movimenti telecamera

	if(!gbToggleCommands){
		if(DXUTIsKeyDown(VK_UP)){

			camera->MoveUP(fElapsedTime);
		}

		if(DXUTIsKeyDown(VK_DOWN)){

			camera->MoveDOWN(fElapsedTime);
		}
	}

	// gestione moviementi luce
	else{

		if(DXUTIsKeyDown(VK_RIGHT))
				if(g_LightX < 5.0004f){
				g_LightX+=0.0001f;
				D3DXMatrixTranslation(&translation,g_LightX/8,0.0f,0.0f);
				D3DXMatrixMultiply(&g_WorldLight1, &g_WorldLight1, &translation);
				}

				
			if(DXUTIsKeyDown(VK_LEFT))
				if(g_LightX > 4.998f){
				g_LightX-=0.0001f;
				D3DXMatrixTranslation(&translation,-g_LightX/8,0.0f,0.0f);
				D3DXMatrixMultiply(&g_WorldLight1, &g_WorldLight1, &translation);
				}
				
			if(DXUTIsKeyDown(VK_UP))
				if(g_LightZ < 4.011f){
					g_LightZ+=0.0001f;
					D3DXMatrixTranslation(&translation,0.0f,0.0f,g_LightZ/8);
					D3DXMatrixMultiply(&g_WorldLight1, &g_WorldLight1, &translation);
				}
				
			if(DXUTIsKeyDown(VK_DOWN))
				if(g_LightZ > 3.99f){
					g_LightZ-=0.0001f;
					D3DXMatrixTranslation(&translation,0.0f,0.0f,-g_LightZ/8);
					D3DXMatrixMultiply(&g_WorldLight1, &g_WorldLight1, &translation);
				}
				

	}

	static bool bMoving = false;
	static POINT prevCoord;
	if(DXUTIsMouseButtonDown(VK_LBUTTON)){
		POINT coord;
		GetCursorPos(&coord);
		if(bMoving){

			// Moto relativo a X e Y
			float motionX = (float)coord.x - (float)prevCoord.x;
			float motionY = (float)coord.y - (float)prevCoord.y;

			// matrici di supporto per la rotazione
			D3DXMATRIX rotationX;
			D3DXMATRIX rotationY;

			D3DXMatrixRotationY(&rotationX, DEG2RAD(motionX));
			D3DXMatrixMultiply(&g_WorldTeapot, &g_WorldTeapot, &rotationX);

			
			D3DXMatrixRotationX(&rotationY, DEG2RAD(motionY));
			D3DXMatrixMultiply(&g_WorldTeapot, &g_WorldTeapot, &rotationY);


		}
		prevCoord = coord;
		bMoving = true;
	}
	else{
		bMoving = false;
	}

}


//--------------------------------------------------------------------------------------
// Gestisce coda messaggi
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
    return 0;
}

//--------------------------------------------------------------------------------------
// Gestisce eventi da tastiera
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	
	
	D3DXMATRIX translation;
    if( bKeyDown )
    {
        switch( nChar )
        {
            case '1':
				if(g_Light1.mColour.x<upperLightLimit){
					g_Light1.mColour.x += 0.1f;
					g_Light1.mColour.y += 0.1f;
					g_Light1.mColour.z += 0.2f;
				}
				break;
			case '2':
				if(g_Light1.mColour.x>lowerLightLimit){
					g_Light1.mColour.x -= 0.1f;
					g_Light1.mColour.y -= 0.1f;
					g_Light1.mColour.z -= 0.2f;
				}
				break;
				
            case 'F': 
				gbToggleFps = !gbToggleFps;
				break;
			case VK_F10:
			    DXUTToggleFullScreen();
				break;	
			
			case 'L':
				if(gbToggleLights == true)
					gbToggleLights = false;
				else
					gbToggleLights = true;
				break;

			case 'C':
				gbToggleCommands=!gbToggleCommands;
				break;

        }
    }
}

//--------------------------------------------------------------------------------------
// Setta il materiale: Effettua i collegamenti necessari con le variabili dell'effetto.
//--------------------------------------------------------------------------------------
void SetMaterial(sMaterial &MeshMaterial){

	phongShader->GetEffect()->GetVariableByName("g_material")->GetMemberByName("Kd")->AsVector()->SetFloatVector(MeshMaterial.mKd);
	phongShader->GetEffect()->GetVariableByName("g_material")->GetMemberByName("Ka")->AsVector()->SetFloatVector(MeshMaterial.mKa);
	phongShader->GetEffect()->GetVariableByName("g_material")->GetMemberByName("Ke")->AsVector()->SetFloatVector(MeshMaterial.mKe);
	phongShader->GetEffect()->GetVariableByName("g_material")->GetMemberByName("Ks")->AsVector()->SetFloatVector(MeshMaterial.mKs);
	phongShader->GetEffect()->GetVariableByName("g_material")->GetMemberByName("Shininess")->AsScalar()->SetFloat(MeshMaterial.mShininess);

}


//--------------------------------------------------------------------------------------
// Aggancia le luci all'effetto, collegando le variabili
//--------------------------------------------------------------------------------------
void SetupLights(){
	
	D3DXVECTOR3 poslight1(g_WorldLight1._41,g_WorldLight1._42, g_WorldLight1._43 );
	phongShader->GetEffect()->GetVariableByName("g_lights")->GetElement(0)->GetMemberByName("Position")->AsVector()->SetFloatVector(poslight1);
	phongShader->GetEffect()->GetVariableByName("g_lights")->GetElement(0)->GetMemberByName("Color")->AsVector()->SetFloatVector(g_Light1.mColour);
	phongShader->GetEffect()->GetVariableByName("g_lights")->GetElement(0)->GetMemberByName("Ambient")->AsVector()->SetFloatVector(g_Light1.mAmbientColour);
	phongShader->GetEffect()->GetVariableByName("g_lights")->GetElement(0)->GetMemberByName("Attenuation")->AsVector()->SetFloatVector(g_Light1.mAttenuation);


}





