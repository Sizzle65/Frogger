#include "Car.h"

using namespace Simplex;

Car::Car(vector3 a_v3StartingPos, String a_sModelPath, float a_fSpeed)
{
	m_v3StartingPos = a_v3StartingPos;
	m_v3Position = a_v3StartingPos;
	m_fSpeed = a_fSpeed;
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

	m_fLeftBound = -10.0f;
	m_fRightBound = 10.0f;
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

	//check against left bound
	if (m_v3Position.x < m_fLeftBound)
		return true;

	return false;
}

void Simplex::Car::ResetPosition()
{
	m_v3Position = m_v3StartingPos;
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
	m_v3Position.x += m_fSpeed;

	//reset position if out of bounds
	if (IsOutOfBounds())
		ResetPosition();

	//set up to render car
	matrix4 mCar = glm::translate(m_v3Position);
	SetModelMatrix(mCar);
	m_pMeshMngr->AddAxisToRenderList(mCar);

	m_pCarModel->AddToRenderList();
	m_pRigidBody->AddToRenderList();
}

Car & Simplex::Car::operator=(Car const & input)
{
	// TODO: insert return statement here
	return *this;
}
