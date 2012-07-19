//--------------------------------------------------------------------------------------
// File: CameraManager.h
// 
// **************************************
// Header classe per definizione manager camera per la gestione del punto di vista della scena 
// **************************************
//
// Copyright (c) Volpin Jacopo 2010. Do not distribute. (Master Game Development - University of Verona)
//--------------------------------------------------------------------------------------


#pragma once
#include "../DXUT/DXUT.h"

class CameraManager{
public:
	CameraManager();
	void Initalize(D3DXVECTOR3 viewEye,D3DXVECTOR3 viewUp,D3DXVECTOR3 viewAt);

	float GetCameraSpeed();
	float GetCameraAngleSpeed();
	D3DXVECTOR3 GetViewUpVector();
	D3DXVECTOR3 GetViewEyeVector();
	D3DXVECTOR3 GetViewAtVector();
	D3DXVECTOR3 GetViewViewVector();
	D3DXVECTOR3 GetViewRightVector();

	D3DXMATRIX GetViewMatrix();
	D3DXMATRIX GetProjectionMatrix();

	void MoveUP(float elapsedTime);
	void MoveDOWN(float elapsedTime);

private:
	float m_CameraSpeed;
    float m_CameraAngleSpeed;
	D3DXVECTOR3 m_ViewUpVector; // Vettore Up
	D3DXVECTOR3 m_ViewEyeVector; // Posizione
    D3DXVECTOR3 m_ViewAtVector; // Punto fissato
    D3DXVECTOR3 m_ViewViewVector; // Vettore vista - At = Eye + View
    D3DXVECTOR3 m_ViewRightVector; // Vettore dx

	
	// Matrici di trasformazione
	D3DXMATRIX  m_View;
	D3DXMATRIX  m_Projection;
	
};