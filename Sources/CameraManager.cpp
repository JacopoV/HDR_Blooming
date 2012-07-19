#include "../Headers/CameraManager.h"

CameraManager::CameraManager()
{
	m_CameraSpeed = 10.0f;
	m_CameraAngleSpeed = 0.1f;
	D3DXMatrixIdentity(&m_View);
	D3DXMatrixIdentity(&m_Projection);

}

void CameraManager::Initalize(D3DXVECTOR3 viewEye,D3DXVECTOR3 viewUp,D3DXVECTOR3 viewAt){

	m_ViewEyeVector = viewEye;
	m_ViewUpVector = viewUp;
	m_ViewAtVector = viewAt;

	// Il vettore vista è semplicemente eye - at normalizzato
	m_ViewViewVector = (m_ViewAtVector - m_ViewEyeVector); 
	D3DXVec3Normalize(&m_ViewViewVector, &m_ViewViewVector);

	// Il vettore dx è ottenuto come cross-product del vettore up e del vettore vista. Anch'esso è normalizzato
	D3DXVec3Cross(&m_ViewRightVector,&m_ViewUpVector,&m_ViewViewVector);

	// Costruisce matrice di view
	D3DXMatrixLookAtLH( &m_View, &m_ViewEyeVector, &m_ViewAtVector, &m_ViewUpVector );

	// Initialize the projection matrix
	D3DXMatrixPerspectiveFovLH( &m_Projection, ( float )D3DX_PI * 0.25f,((float)DXUTGetWindowWidth() / (float)DXUTGetWindowHeight()), 0.1f, 100.0f);

};

float CameraManager::GetCameraSpeed(){
	
	return m_CameraSpeed;
};

float CameraManager::GetCameraAngleSpeed(){

	return m_CameraAngleSpeed;
};

D3DXVECTOR3 CameraManager::GetViewUpVector(){

	return m_ViewUpVector;
};
	
D3DXVECTOR3 CameraManager::GetViewEyeVector(){

	return m_ViewEyeVector;
};
	
D3DXVECTOR3 CameraManager::GetViewAtVector(){

	return m_ViewAtVector;
};
	
D3DXVECTOR3 CameraManager::GetViewViewVector(){

	return m_ViewViewVector;
};
	
D3DXVECTOR3 CameraManager::GetViewRightVector(){

	return m_ViewRightVector;
};

	
D3DXMATRIX CameraManager::GetViewMatrix(){

	return m_View;
};
	
D3DXMATRIX CameraManager::GetProjectionMatrix(){

	return m_Projection;
};

void CameraManager::MoveUP(float elapsedTime){

	// Spostamento positivo rispetto alla direzione di vista (avanti)
		m_ViewEyeVector += elapsedTime * m_CameraSpeed * m_ViewViewVector;
		m_ViewAtVector += elapsedTime * m_CameraSpeed * m_ViewViewVector;
		D3DXMatrixLookAtLH( &m_View, &m_ViewEyeVector, &m_ViewAtVector, &m_ViewUpVector );
};

void CameraManager::MoveDOWN(float elapsedTime){

	// Spostamento negativo rispetto alla direzione di vista (indietro)
		m_ViewEyeVector -= elapsedTime * m_CameraSpeed* m_ViewViewVector;
		m_ViewAtVector -= elapsedTime * m_CameraSpeed * m_ViewViewVector;
		D3DXMatrixLookAtLH( &m_View, &m_ViewEyeVector, &m_ViewAtVector, &m_ViewUpVector );

};


