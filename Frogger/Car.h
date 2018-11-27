#pragma once
#ifndef __CARCLASS_H_
#define __CARCLASS_H_

#include "MyRigidBody.h"

namespace Simplex
{
class Car
{
	vector3 m_v3Position = vector3(0.0f, 0.0f, 0.0f); //current position
	vector3 m_v3StartingPos = vector3(0.0f, 0.0f, 0.0f); //car's starting position
	
	float m_fLeftBound = 0.0f; //left bound of game
	float m_fRightBound = 0.0f; //right bound of game

	float m_fSpeed = 0.0f; //car's speed, use negatives to adjust direction
	float m_sDirection;

	matrix4 m_m4ModelMatrix = IDENTITY_M4; //model matrix
	Model* m_pCarModel = nullptr; //model to use for the car
	String m_sModelPath; //filepath to model

	MyRigidBody* m_pRigidBody = nullptr; //the car's rigid body
private:
	MeshManager* m_pMeshMngr = nullptr; //Mesh Manager
public:
	/*
	USAGE: constructor
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	Car(vector3 a_v3StartingPos, String a_sModelPath, float a_fSpeed = 1.0f);
	/*
	USAGE: destructor
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	~Car();
	/*
	USAGE: initialization
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Init();
	/*
	USAGE: release
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Release();
	/*
	USAGE: checks if the car is colliding with a given rigid body
	ARGUMENTS: the rigid body to check
	OUTPUT: a bool indicating whether a collision occurred
	*/
	bool IsColliding(MyRigidBody &other);
	/*
	USAGE: checks if the car has reached the end of the map
	ARGUMENTS: ---
	OUTPUT: a bool indicating whether the car is out of bounds
	*/
	bool IsOutOfBounds();
	/*
	USAGE: resets car to starting location
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void ResetPosition();
	/*
	USAGE: sets the model matrix for the model and the rigidbody
	ARGUMENTS: Matrix4 a_m4ModelMatrix -> model matrix to set
	OUTPUT: ---
	*/
	void SetModelMatrix(matrix4 a_m4ModelMatrix);
	/*
	USAGE: updates the car
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Update();
	/*
	USAGE: returns the rigid body
	ARGUMENTS: ---
	OUTPUT: this car's rigid body
	*/
	MyRigidBody* GetRigidBody() { return m_pRigidBody; }
private:
	/*
	USAGE: copy constructor
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	Car(Car const& input);
	/*
	USAGE: assignment operator
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	Car& operator=(Car const& input);
}; //end class
} //end simplex namespace

#endif //__CARCLASS_H_

/*
USAGE:
ARGUMENTS: ---
OUTPUT: ---
*/

