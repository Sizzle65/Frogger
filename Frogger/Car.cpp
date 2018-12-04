#include "Car.h"

using namespace Simplex;

Car::Car(vector3 a_v3StartingPos, String a_sModelPath,int a_row, float a_fSpeed)
{
	m_v3StartingPos = a_v3StartingPos;
	m_v3Position = a_v3StartingPos;
	m_fDefaultSpeed = a_fSpeed;
	m_iCarRow = a_row;
	m_fSpeed = (m_fDefaultSpeed * (rand()%50 + m_fSpeedMin));
	m_sModelPath = a_sModelPath;
	Init();
}

Car::~Car()
{
	Release();
}

void Simplex::Car::Init()
{
	m_pCarModel = new Model();
	m_pCarModel->Load(m_sModelPath);
	m_pRigidBody = new MyRigidBody(m_pCarModel->GetVertexList());

	m_pMeshMngr = MeshManager::GetInstance();
	m_fLeftBound = -8.0f;
	m_fRightBound = 17.0f;
	if (m_v3StartingPos.x == m_fRightBound) {
		m_sDirection = 1.0f;
	}
	else {
		m_sDirection = -1.0f;
	}
}

void Simplex::Car::Release()
{
	SafeDelete(m_pRigidBody);
}

bool Simplex::Car::IsColliding(MyRigidBody & other)
{
	return m_pRigidBody->IsColliding(&other);
}

bool Simplex::Car::IsOutOfBounds()
{
	//check against right bound
	if (m_v3Position.x > m_fRightBound)
		return true;
	//else if (m_v3Position.x > m_fRightBound + 4.0f && m_sDirection)
	//	return true;

	//check against left bound
	if (m_v3Position.x < m_fLeftBound)
		return true;
	else if (m_v3Position.x < m_fLeftBound + 2.0f && m_sDirection == 1.0f)
		return true;


	return false;
}

void Simplex::Car::ResetPosition()
{
	m_v3Position = m_v3StartingPos;
	// Resets the speed to add variation
	m_fSpeed = (m_fDefaultSpeed * (rand() % 50 + m_fSpeedMin));

}

void Simplex::Car::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	m_m4ModelMatrix = a_m4ModelMatrix;

	m_pCarModel->SetModelMatrix(a_m4ModelMatrix);
	m_pRigidBody->SetModelMatrix(a_m4ModelMatrix);
}

void Simplex::Car::Update()
{
	//move car along
	m_v3Position.x += (m_sDirection * m_fSpeed);

	//reset position if out of bounds
	if (IsOutOfBounds())
		ResetPosition();

	//set up to render car
	matrix4 mCar = glm::translate(m_v3Position);
	matrix4 carRot = glm::rotate(IDENTITY_M4, glm::radians(90.0f), AXIS_Z);

	SetModelMatrix(mCar * carRot);
	//m_pMeshMngr->AddAxisToRenderList(mCar);

	m_pCarModel->AddToRenderList();
	m_pRigidBody->AddToRenderList();
}

Car & Simplex::Car::operator=(Car const & input)
{
	// TODO: insert return statement here
	return *this;
}
