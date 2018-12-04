#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Frogger - Team Frogger";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 3.0f, 8.0f), //Position
		vector3(0.0f, 3.0f, 7.0f),	//Target
		AXIS_Y);					//Up
	m_pCameraMngr->SetCameraMode(CAM_ORTHO_Z);

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light(0 is reserved for global light)


	//creeper
	m_pCreeper = new Model();
	m_pCreeper->Load("Minecraft\\Creeper.obj");
	m_pCreeperRB = new MyRigidBody(m_pCreeper->GetVertexList());
	m_v3Creeper.x = m_v3PlrStart.x;
	m_v3Creeper.y = m_v3PlrStart.y;
	m_v3Creeper.z = m_v3PlrStart.z;

	//steve
	//m_pSteve = new Model();
	//m_pSteve->Load("Minecraft\\Steve.obj");
	//m_pSteveRB = new MyRigidBody(m_pSteve->GetVertexList());

	//set up cars
	m_pCarList = std::vector<Car*>();
	for (uint i = 0; i < 200; i++)
	{
		if (i % 2 == 0) {
			Car* thisCar = new Car(
				vector3(17.0f, -6.5f + 2 * i, 0.0f),
				"Minecraft\\Steve.obj",
				i*2 + 3,
				-0.01f);
			m_pCarList.push_back(thisCar);
		}
		else {
			Car* thisCar = new Car(
				vector3(-6.0f, -6.5f + 2 * i, 0.0f),
				"Minecraft\\Steve.obj",
				i*2 + 3,
				-0.01f);
			m_pCarList.push_back(thisCar);
		}
		

		

		
	}

	//White color for grid
	std::vector<vector3> white;
	for (size_t i = 0; i < 6; i++)
	{
		white.push_back(C_GRAY);
	}

	//black color for grid
	std::vector<vector3> black;
	for (size_t i = 0; i < 6; i++)
	{
		black.push_back(C_BLACK);
	}
	/*
	//generate checkerboard
	bool isWhite = true;
	for (int i = -10; i < 65; i++)
	{
		for (int j = -8; j < 17; j++)
		{
			Mesh* mesh = new Mesh();
			if (isWhite)
			{
				mesh->SetColorList(white);
				mesh->AddQuad(vector3(j, i, 0), vector3(j + 1, i, 0), vector3(j, i + 1, 0), vector3(j + 1, i + 1, 0));
			}
			else
			{
				mesh->SetColorList(black);
				mesh->AddQuad(vector3(j, i, 0), vector3(j + 1, i, 0), vector3(j, i + 1, 0), vector3(j + 1, i + 1, 0));
			}
			isWhite = !isWhite;
			mesh->CompileOpenGL3X();
			tiles.push_back(mesh);
		}
	}*/

	// Adds a single quad instead of a checkerboard
	Mesh* mesh = new Mesh();
	mesh->SetColorList(white);
	mesh->AddQuad(vector3(-8, -10, 0), vector3(17, -10, 0), vector3(-8, 400, 0), vector3(17, 400, 0));
	mesh->CompileOpenGL3X();
	tiles.push_back(mesh);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	//ArcBall();

	//Is the first person camera active?
	//CameraRotation();

	//Set model matrix to the creeper
	matrix4 creeperRotation = glm::rotate(IDENTITY_M4,glm::radians(90.0f),AXIS_X);
	matrix4 mCreeper = glm::translate(m_v3Creeper) * creeperRotation * ToMatrix4(m_qCreeper) * ToMatrix4(m_qArcBall);
	
	m_pCreeper->SetModelMatrix(mCreeper);
	m_pCreeperRB->SetModelMatrix(mCreeper);
	m_pMeshMngr->AddAxisToRenderList(mCreeper);

	//Set model matrix to Steve
	/*matrix4 mSteve = glm::translate(vector3(2.25f, 0.0f, 0.0f));
	m_pSteve->SetModelMatrix(mSteve);
	m_pSteveRB->SetModelMatrix(mSteve);
	m_pMeshMngr->AddAxisToRenderList(mSteve);*/

	bool bColliding = false;

	for (int i = 0; i < m_pCarList.size(); i++)
	{
		//Spatial optimization
		if (m_bSpatial)
		{
			Car* thisCar = m_pCarList[i];

			MyRigidBody* thisCarRB = thisCar->GetRigidBody();

			int thisCarRow = thisCar->GetRow();
			int rowDif = m_iCreeperRow - thisCarRow;
			if (rowDif < 0)
				rowDif *= -1;

			if (rowDif < 30)
				thisCar->Update();

			if (m_iCreeperRow == thisCarRow)
			{
				bColliding = m_pCreeperRB->IsColliding(thisCarRB);
				m_pCreeperRB->RemoveCollisionWith(thisCarRB);
				thisCarRB->RemoveCollisionWith(m_pCreeperRB);
			}
		}
		else
		{
			m_pCarList[i]->Update();

			bColliding = m_pCreeperRB->IsColliding(m_pCarList[i]->GetRigidBody());
		}

		if (bColliding) {
			m_iDeaths++;
			break;
		}
	}

	//bool bColliding = m_pCreeperRB->IsColliding(m_pSteveRB);

	m_pCreeper->AddToRenderList();
	m_pCreeperRB->AddToRenderList();

	/*m_pSteve->AddToRenderList();
	m_pSteveRB->AddToRenderList();*/
	//m_pSteveRB->AddToRenderList(m_pCreeperRB);

	m_pMeshMngr->Print("Colliding: ");
	if (bColliding)
	{
		m_pMeshMngr->PrintLine("YES!", C_RED);

		m_v3Creeper = m_v3PlrStart;
		m_pCameraMngr->MoveVertical(-m_fCameraReset);
		m_fCameraReset = 0.0f;
		m_iCreeperRow = 0;
		bColliding = false;

	}
	else
		m_pMeshMngr->PrintLine("no", C_YELLOW);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	// draw a skybox
	//m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	for (size_t i = 0; i < tiles.size(); i++)
	{
		tiles[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), ToMatrix4(m_qArcBall));
	}

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}

void Application::Release(void)
{

	//release the model
	SafeDelete(m_pCreeper);

	//release the rigid body for the model
	SafeDelete(m_pCreeperRB);

	//release the model
	SafeDelete(m_pSteve);

	//release the rigid body for the model
	SafeDelete(m_pSteveRB);

	tiles.clear();

	//release GUI
	ShutdownGUI();
}