#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if(SAT(a_pOther) != eSATResults::SAT_NONE)
			bColliding = false;// reset to false
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	AddToRenderList(a_pOther);
	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

void Simplex::MyRigidBody::AddToRenderList(MyRigidBody* const a_pOther)
{
	if (m_bVisibleSAT)
	{
		if (m_CollidingRBSet.size() == 0)
		{
			//std::cout << result << std::endl;
			matrix4 mat = glm::translate((a_pOther->GetCenterGlobal() - GetCenterGlobal()) / 2 + GetCenterGlobal());
			vector3 color = C_RED;
			if (result == eSATResults::SAT_NONE) //no planes checked
			{
				mat *= glm::scale(vector3(0, 0, 0));
			}
			else if (result == eSATResults::SAT_AX) //left or right
			{
				mat *= glm::scale(vector3(0, 3, 3));
				color = C_RED;
			}
			else if (result == eSATResults::SAT_AY) //up or down
			{
				mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				mat *= glm::scale(vector3(0, 3, 3));
				color = C_GREEN;
			}
			else if (result == eSATResults::SAT_AZ) //forward or backward
			{
				mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				mat *= glm::scale(vector3(0, 3, 3));
				color = C_BLUE;
			}
			else if (result == eSATResults::SAT_AXxBX)
			{
				mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				mat *= glm::scale(vector3(0, 3, 3));
				color = C_BLACK;
			}
			else if (result == eSATResults::SAT_AXxBY)
			{
				mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				mat *= glm::scale(vector3(0, 3, 3));
				color = C_BLACK;
			}
			else if (result == eSATResults::SAT_AXxBZ)
			{
				mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				mat *= glm::scale(vector3(0, 3, 3));
				color = C_BLACK;
			}
			else if (result == eSATResults::SAT_AYxBX)
			{
				mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				mat *= glm::scale(vector3(0, 3, 3));
				color = C_BLACK;
			}
			else if (result == eSATResults::SAT_AYxBY)
			{
				mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				mat *= glm::scale(vector3(0, 3, 3));
				color = C_BLACK;
			}
			else if (result == eSATResults::SAT_AZxBX)
			{
				mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				mat *= glm::scale(vector3(0, 3, 3));
				color = C_BLACK;
			}
			else if (result == eSATResults::SAT_AZxBY)
			{
				mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				mat *= glm::scale(vector3(0, 3, 3));
				color = C_BLACK;
			}
			else if (result == eSATResults::SAT_AZxBZ)
			{
				mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				mat *= glm::scale(vector3(0, 3, 3));
				color = C_BLACK;
			}
			m_pMeshMngr->AddCubeToRenderList(mat, color);
		}
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	//I referenced this github repo for most of this section https://github.com/Herm20/FreeFall/blob/master/Freefall/MyRigidBody.cpp
	//I did a lot of consolidation and improvements such as making a helper function

	//15 total checks: 6 checks using X,Y,Z coords, 9 using cross product to get projection
	//X,Y,Z of first object
	vector3 axisX = glm::normalize(v3Corner[1] - v3Corner[0]);
	vector3 axisY = glm::normalize(v3Corner[2] - v3Corner[0]);
	vector3 axisZ = glm::normalize(v3Corner[4] - v3Corner[0]);

	//X,Y,Z of second object
	vector3 axisX2 = glm::normalize(a_pOther->v3Corner[1] - a_pOther->v3Corner[0]);
	vector3 axisY2 = glm::normalize(a_pOther->v3Corner[2] - a_pOther->v3Corner[0]);
	vector3 axisZ2 = glm::normalize(a_pOther->v3Corner[4] - a_pOther->v3Corner[0]);

	//check if objects are colliding using X,Y,Z
	if (SATHelper(axisX, a_pOther)) {
		result = eSATResults::SAT_AX; 
		return eSATResults::SAT_AX;
	}
	if (SATHelper(axisY, a_pOther)) {
		result = eSATResults::SAT_AY; 
		return eSATResults::SAT_AY;
	}
	if (SATHelper(axisZ, a_pOther)) {
		result = eSATResults::SAT_AZ; 
		return eSATResults::SAT_AZ;
	}
	if (SATHelper(axisX2, a_pOther)) {
		result = eSATResults::SAT_AX; 
		return eSATResults::SAT_AX;
	}
	if (SATHelper(axisY2, a_pOther)) {
		result = eSATResults::SAT_AY; 
		return eSATResults::SAT_AY;
	}
	if (SATHelper(axisZ2, a_pOther)) {
		result = eSATResults::SAT_AZ; 
		return eSATResults::SAT_AZ;
	}
	//cross products of axis of both
	vector3 crossXX = glm::cross(axisX, axisX2);
	vector3 crossXY = glm::cross(axisX, axisY2);
	vector3 crossXZ = glm::cross(axisX, axisZ2);

	vector3 crossYX = glm::cross(axisY, axisX2);
	vector3 crossYY = glm::cross(axisY, axisY2);
	vector3 crossYZ = glm::cross(axisY, axisZ2);

	vector3 crossZX = glm::cross(axisZ, axisX2);
	vector3 crossZY = glm::cross(axisZ, axisY2);
	vector3 crossZZ = glm::cross(axisZ, axisZ2);

	// putting cross product vectors in array
	vector3 cross[9];
	cross[0] = crossXX;
	cross[1] = crossXY;
	cross[2] = crossXZ;

	cross[3] = crossYX;
	cross[4] = crossYY;
	cross[5] = crossYZ;

	cross[6] = crossZX;
	cross[7] = crossZY;
	cross[8] = crossZZ;

	//check if objects are colliding using cross product
	for (int j = 0; j < 9; j++)
	{
		float min = 10000;
		float max = -10000;
		float min2 = 10000;
		float max2 = -10000;

		if (cross[j] != ZERO_V3)
		{
			for (int i = 0; i < 8; i++)
			{
				// get the dot product of all the points in the array
				float temp = glm::dot(cross[j], v3Corner[i]);
				float temp2 = glm::dot(cross[j], a_pOther->v3Corner[i]);

				if (temp <= min)
					min = temp;

				if (temp >= max)
					max = temp;
				
				if (temp2 <= min2)
					min2 = temp2;

				if (temp2 >= max2)
					max2 = temp2;
			}
			if (min >= max2 || min2 >= max)
			{
				if (j == 0) {
					result = eSATResults::SAT_AXxBX; 
					return eSATResults::SAT_AXxBX;
				}
				if (j == 1) {
					result = eSATResults::SAT_AXxBY; 
					return eSATResults::SAT_AXxBY;
				}
				if (j == 2) {
					result = eSATResults::SAT_AXxBZ; 
					return eSATResults::SAT_AXxBZ;
				}
				if (j == 3) {
					result = eSATResults::SAT_AYxBX; 
					return eSATResults::SAT_AYxBX;
				}
				if (j == 4) {
					result = eSATResults::SAT_AYxBY; 
					return eSATResults::SAT_AYxBY;
				}
				if (j == 5) {
					result = eSATResults::SAT_AXxBZ; 
					return eSATResults::SAT_AXxBZ;
				}
				if (j == 6) {
					result = eSATResults::SAT_AZxBX; 
					return eSATResults::SAT_AZxBX;
				}
				if (j == 7) {
					result = eSATResults::SAT_AZxBY; 
					return eSATResults::SAT_AZxBY;
				}
				if (j == 8) {
					result = eSATResults::SAT_AZxBZ; 
					return eSATResults::SAT_AZxBZ;
				}
			}
		}
	}

	//there is no axis test that separates this two objects
	result = 0;
	return eSATResults::SAT_NONE;
}
bool MyRigidBody::SATHelper(vector3 axis, MyRigidBody* const a_pOther) {
	//starting max and min values
	float min = 100000;
	float max = -100000;
	float min2 = 100000;
	float max2 = -100000;

	//check if objects are colliding
	for (int i = 0; i < 8; i++)
	{
		//dot product of all the points in array
		float temp = glm::dot(axis, v3Corner[i]);
		float temp2 = glm::dot(axis, a_pOther->v3Corner[i]);

		if (temp <= min)
			min = temp;

		if (temp >= max)
			max = temp;
		
		if (temp2 <= min2)
			min2 = temp2;

		if (temp2 >= max2)
			max2 = temp2;
	}
	return (min >= max2 || min2 >= max) ? true : false; //not colliding
}