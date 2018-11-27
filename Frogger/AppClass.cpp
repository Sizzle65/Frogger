#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Frogger - placeholder@rit.edu";

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
	m_v3Creeper.y = -10;

	//steve
	m_pSteve = new Model();
	m_pSteve->Load("Minecraft\\Steve.obj");
	m_pSteveRB = new MyRigidBody(m_pSteve->GetVertexList());

	//White color for grid
	std::vector<vector3> white;
	for (size_t i = 0; i < 6; i++)
	{
		white.push_back(C_WHITE);
	}

	//black color for grid
	std::vector<vector3> black;
	for (size_t i = 0; i < 6; i++)
	{
		black.push_back(C_BLACK);
	}

	//generate checkerboard
	bool isWhite = true;
	for (int i = -10; i < 9; i++)
	{
		for (int j = -10; j < 9; j++)
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
	}

}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Set model matrix to the creeper
	matrix4 mCreeper = glm::translate(m_v3Creeper) * ToMatrix4(m_qCreeper) * ToMatrix4(m_qArcBall);
	m_pCreeper->SetModelMatrix(mCreeper);
	m_pCreeperRB->SetModelMatrix(mCreeper);
	m_pMeshMngr->AddAxisToRenderList(mCreeper);

	//Set model matrix to Steve
	matrix4 mSteve = glm::translate(vector3(2.25f, 0.0f, 0.0f));
	m_pSteve->SetModelMatrix(mSteve);
	m_pSteveRB->SetModelMatrix(mSteve);
	m_pMeshMngr->AddAxisToRenderList(mSteve);

	bool bColliding = m_pCreeperRB->IsColliding(m_pSteveRB);

	m_pCreeper->AddToRenderList();
	m_pCreeperRB->AddToRenderList();

	m_pSteve->AddToRenderList();
	m_pSteveRB->AddToRenderList();
	//m_pSteveRB->AddToRenderList(m_pCreeperRB);

	m_pMeshMngr->Print("Colliding: ");
	if (bColliding)
		m_pMeshMngr->PrintLine("YES!", C_RED);
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