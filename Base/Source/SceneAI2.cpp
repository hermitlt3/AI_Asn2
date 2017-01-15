#include "SceneAI2.h"

// Managers
#include "Application.h"
#include "MeshBuilder.h"
#include "KeyboardController.h"
#include "GameObjectManager.h"
#include "MessageBoard.h"

// Libs
#include "GL\glew.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>

#include "Priest.h"
#include "Guardian.h"

using std::ifstream;

SceneAI2::SceneAI2()
{
}

SceneAI2::~SceneAI2()
{
}

void SceneAI2::Init()
{
	SceneBase::Init();
	//Physics code here
	Math::InitRNG();
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	m_objectCount = 0;

	priest = new Priest();
	priest->type = GameObject::GO_PRIEST;
	priest->pos = Vector3(30, 15, 0);
	priest->scale = Vector3(1, 1, 1);
	priest->active = true;
	GameObjectManager::GetInstance()->m_goList.push_back(priest);

	guardian = new Guardian();
	guardian->type = GameObject::GO_GUARDIAN;
	guardian->pos = Vector3(25, 10, 0);
	guardian->scale = Vector3(1, 1, 1);
	guardian->active = true;
	GameObjectManager::GetInstance()->m_goList.push_back(guardian);

}


GameObject* SceneAI2::FetchGO()
{
	for (std::vector<GameObject *>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end(); ++it)
	{
		GameObject *go = *it;
		if (go->active == false)
		{
			go->active = true;
			++m_objectCount;
			return go;
		}
	}
	for (size_t count = 0; count < 10; ++count)
	{
		GameObjectManager::GetInstance()->m_goList.push_back(new GameObject(GameObject::GO_NONE));
	}

	GameObject *go = *(GameObjectManager::GetInstance()->m_goList.end() - 1);
	go->active = true;
	++m_objectCount;
	return go;
}



void SceneAI2::UpdateMouse(double dt)
{
	//Mouse Section
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
	}
}

void SceneAI2::UpdateKeys(double dt)
{
	if (KeyboardController::GetInstance()->IsKeyDown('W')) {
		priest->health -= 5;
	}
	if (KeyboardController::GetInstance()->IsKeyDown('S')) {

	}
	if (KeyboardController::GetInstance()->IsKeyDown('A')) {
		MessageBoard::GetInstance()->BroadcastMessage("INJURED");
	}
	if (KeyboardController::GetInstance()->IsKeyDown('D')) {
		MessageBoard::GetInstance()->BroadcastMessage("UNINJURED");
	}

	//
	double x, y;
	Application::GetCursorPos(&x, &y);
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();
	float mouseX = (float)x * m_worldWidth / w;
	float mouseY = (float)(h - y) * m_worldHeight / h;
}

void SceneAI2::UpdatePhysics(double dt)
{
	//Physics Simulation Section
	for (std::vector<GameObject *>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->active)
			continue;

		go->pos += go->vel * (float)dt;
		float radius = go->scale.x;

		//Exercise 8a: handle collision between GO_BALL and GO_BALL using velocity swap
		for (std::vector<GameObject *>::iterator ho = it + 1; ho != GameObjectManager::GetInstance()->m_goList.end(); ++ho)
		{
			GameObject* other = (GameObject*)*ho;
			if (!other->active)
				continue;

			GameObject* goA = go, *goB = other;
			if (go->type != GameObject::GO_PLAYER)
			{
				if (other->type != GameObject::GO_PLAYER)
					continue;
				goA = other;
				goB = go;
			}
			if (CheckCollision(goA, goB, dt)) {
				CollisionResponse(goA, goB, dt);
				break;
			}
		}
	}
}

void SceneAI2::Update(double dt)
{
	MessageBoard::GetInstance()->Update(dt);
	fps = 1.f / (float)dt;
	priest->Update(dt);

	UpdateKeys(dt);
	UpdateMouse(dt);
	UpdatePhysics(dt);

	priest->FSM();
}

void SceneAI2::RenderGO(GameObject *go)
{
	switch (go->type)
	{
	case GameObject::GO_PRIEST:
	case GameObject::GO_GUARDIAN:
	{
								  modelStack.PushMatrix();
								  modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
								  modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
								  RenderMesh(meshList[PRIEST], false);
								  modelStack.PopMatrix();
								  break;
	}
	}
}

void SceneAI2::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);

	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
		);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	for (std::vector<GameObject *>::iterator it = GameObjectManager::GetInstance()->m_goList.begin(); it != GameObjectManager::GetInstance()->m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}

	float yCoordinates = 58.f;
	std::ostringstream ss;
	for (size_t i = 0; i < MessageBoard::GetInstance()->GetList().size(); ++i)
	{
		ss.str("");
		ss << i << ": Message Board receives \"" << MessageBoard::GetInstance()->GetList()[i] << "\"";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 1, 1, yCoordinates - i * 1.5f);
	}
}

void SceneAI2::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (GameObjectManager::GetInstance()->m_goList.size() > 0)
	{
		GameObject *go = GameObjectManager::GetInstance()->m_goList.back();
		delete go;
		GameObjectManager::GetInstance()->m_goList.pop_back();
	}
}