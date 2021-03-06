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

	//set up cars
	m_pCarList = std::vector<Car*>();
	for (uint i = 0; i < 4000; i++)
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
	std::cout << m_pCarList.size() << std::endl;
	//White color for grid
	std::vector<vector3> grass;
	for (size_t i = 0; i < 6; i++)
	{
		grass.push_back(vector3(0.0f,0.7f,0.0f));
	}

	//black color for grid
	std::vector<vector3> road;
	for (size_t i = 0; i < 6; i++)
	{
		road.push_back(C_GRAY);
	}
	
	for (int i = 3; i < UpperBoundY; i += 2) {
		Mesh* mesh = new Mesh();
		mesh->SetColorList(road);
		mesh->AddQuad(vector3(lowerBoundX, lowerBoundY + i, 0), vector3(UpperBoundX, lowerBoundY + i, 0), vector3(lowerBoundX, lowerBoundY + 1.0f + i, 0), vector3(UpperBoundX, lowerBoundY + i + 1.0f, 0));
		mesh->CompileOpenGL3X();
		tiles.push_back(mesh);
	}

	for (int i = 4; i < UpperBoundY; i += 2) {
		Mesh* mesh = new Mesh();
		mesh->SetColorList(grass);
		mesh->AddQuad(vector3(lowerBoundX, lowerBoundY + i, 0), vector3(UpperBoundX, lowerBoundY + i, 0), vector3(lowerBoundX, lowerBoundY + 1.0f + i, 0), vector3(UpperBoundX, lowerBoundY + i + 1.0f, 0));
		mesh->CompileOpenGL3X();
		tiles.push_back(mesh);
	}
	
	// Adds a single quad instead of a checkerboard
	Mesh* mesh1 = new Mesh();
	mesh1->SetColorList(grass);
	mesh1->AddQuad(vector3(lowerBoundX, lowerBoundY, 0), vector3(UpperBoundX, lowerBoundY, 0), vector3(lowerBoundX, lowerBoundY + 3.0f, 0), vector3(UpperBoundX, lowerBoundY + 3.0f, 0));
	mesh1->CompileOpenGL3X();
	tiles.push_back(mesh1);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Set model matrix to the creeper
	matrix4 creeperRotation = glm::rotate(IDENTITY_M4,glm::radians(90.0f),AXIS_X);
	matrix4 mCreeper = glm::translate(m_v3Creeper) * creeperRotation * ToMatrix4(m_qCreeper) * ToMatrix4(m_qArcBall);
	
	m_pCreeper->SetModelMatrix(mCreeper);
	m_pCreeperRB->SetModelMatrix(mCreeper);

	if (!m_bPauseGame)
	{
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

					if (bColliding)
					{
						m_fHitSpeed = thisCar->GetSpeed();
					}
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
		
		m_pCreeper->AddToRenderList();
		m_pCreeperRB->AddToRenderList();

		m_pMeshMngr->Print("Colliding: ");

		//handle collisions
		if (bColliding)
		{
			m_pMeshMngr->PrintLine("YES!", C_RED);

			m_bPauseGame = true;

			bColliding = false;

			m_fHitYPercentage = rand() % 100 / 100.0f;
		}
		else
			m_pMeshMngr->PrintLine("no", C_YELLOW);
	} 
	else
	{
		m_v3Creeper.x += m_fHitSpeed;
		m_v3Creeper.y += m_fHitSpeed * m_fHitYPercentage;

		m_pCreeper->AddToRenderList();
		m_pCreeperRB->AddToRenderList();

		for (int i = 0; i < m_pCarList.size(); i++)
		{
			//Spatial optimization
			if (m_bSpatial)
			{
				Car* thisCar = m_pCarList[i];

				int thisCarRow = thisCar->GetRow();
				int rowDif = m_iCreeperRow - thisCarRow;
				if (rowDif < 0)
					rowDif *= -1;

				if (rowDif < 30)
					thisCar->AddToRenderList();
			}
			else
			{
				m_pCarList[i]->AddToRenderList();
			}
		}

		//resume game when creeper is off screen
		if (m_v3Creeper.x > UpperBoundX || m_v3Creeper.x < lowerBoundX)
		{
			m_bPauseGame = false;

			m_v3Creeper = m_v3PlrStart;
			m_pCameraMngr->MoveVertical(-m_fCameraReset);
			m_fCameraReset = 0.0f;
			m_iCreeperRow = 0;
			m_iScore = 0;
			m_fHitSpeed = 0.0f;
		}
	}
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