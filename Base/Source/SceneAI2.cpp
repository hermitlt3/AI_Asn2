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
#include "Enemy.h"
#include "Grid.h"

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
	priest->SetGO(GameObject::GO_PRIEST, Vector3(5, 5, 5), Vector3(0, 0, 0), Vector3(100, 50, 0)); // TYPE, SCALE, ROTATION, POSITION
	GameObjectManager::GetInstance()->m_goList.push_back(priest);

	guardian = new Guardian();
	guardian->SetGO(GameObject::GO_GUARDIAN, Vector3(5, 5, 5), Vector3(0, 0, 0), Vector3(80, 40, 0)); // TYPE, SCALE, ROTATION, POSITION
	guardian->SetOriginalPosition(Vector3(80, 40, 0));
	guardian->health = 100;
	guardian->maxhealth = 100;
	GameObjectManager::GetInstance()->m_goList.push_back(guardian);
	guardian->SetPriest(priest);
	priest->SetGuardian(guardian);

	bossEnemy = new Enemy();
	bossEnemy->SetGO(GameObject::GO_ENEMY, Vector3(5, 5, 5), Vector3(0, 0, 0), Vector3(100, 35, 0)); // TYPE, SCALE, ROTATION, POSITION
	bossEnemy->health = 1000;
	bossEnemy->SetIsLeader(true);
	GameObjectManager::GetInstance()->m_goList.push_back(bossEnemy);

	for (int i = 0; i < 20; ++i) {
		Grid* grid = new Grid(Vector3(m_worldWidth / (20 - i), m_worldHeight / (20 - i), 1), Vector3(m_worldWidth / 20, m_worldHeight / 20), Grid::EMPTY);
		gridList.push_back(grid);
	}
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
		//guardian->health -= 1;
		MessageBoard::GetInstance()->BroadcastMessage("INJURED");
	}
	if (KeyboardController::GetInstance()->IsKeyDown('S')) {
		MessageBoard::GetInstance()->BroadcastMessage("UNINJURED");
	}
	if (KeyboardController::GetInstance()->IsKeyDown('A')) {
		MessageBoard::GetInstance()->BroadcastMessage("UNHARMED");
	}
	if (KeyboardController::GetInstance()->IsKeyDown('D')) {
		priest->health -= 1;
	}
	if (KeyboardController::GetInstance()->IsKeyDown('P'))
	{

	}
	if (KeyboardController::GetInstance()->IsKeyDown('O'))
	{
		bossEnemy->health = 0;
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

		if (go->pos.x < -go->scale.x)
			go->pos.x = m_worldWidth + go->scale.x;
		else if (go->pos.x > m_worldWidth + go->scale.x)
			go->pos.x = 0;
		if (go->pos.y < -go->scale.y)
			go->pos.y = m_worldHeight + go->scale.y;
		else if (go->pos.y > m_worldHeight + go->scale.y)
			go->pos.y = 0;

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
	bossEnemy->Update(dt);
	guardian->Update(dt);

	UpdateKeys(dt);
	UpdateMouse(dt);
	UpdatePhysics(dt);

	priest->FSM();
	guardian->FSM();
	bossEnemy->FSM();
}

void SceneAI2::RenderGO(GameObject *go)
{
	switch (go->type)
	{
	case GameObject::GO_PRIEST:
	{
								  modelStack.PushMatrix();
								  modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
								  modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
								  RenderMesh(meshList[PRIEST], false);
								  modelStack.PopMatrix();
								  break;
	}
	case GameObject::GO_GUARDIAN:
	{
									modelStack.PushMatrix();
									modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
									modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
									RenderMesh(meshList[GUARDIAN], false);
									modelStack.PopMatrix();
									break;
	}
	case GameObject::GO_ENEMY:
	{
								  modelStack.PushMatrix();
								  modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
								  modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
								  RenderMesh(meshList[BOSS_ENEMY], false);
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
	for (int i = 0; i < gridList.size(); ++i) {
		modelStack.PushMatrix();
		modelStack.Translate(gridList[i]->pos.x, gridList[i]->pos.y, gridList[i]->pos.z);
		modelStack.Scale(gridList[i]->scale.x, gridList[i]->scale.y, gridList[i]->scale.z);
		RenderMesh(meshList[GRID], false);
		modelStack.PopMatrix();
	}

	float yCoordinates = 58.f;
	std::ostringstream ss;
	for (size_t i = 0; i < MessageBoard::GetInstance()->GetList().size(); ++i)
	{
		ss.str("");
		ss << i << ": Message Board receives \"" << MessageBoard::GetInstance()->GetList()[i] << "\"";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 1, yCoordinates - i * 1.5f);
	}

	std::ostringstream ss2;
	ss2.str("");
	ss2 << "Priest state: " << priest->GetState();
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 0), 1, 50, 58);
	ss2.str("");
	ss2 << "Guardian state: " << guardian->GetState();
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 0), 1, 50, 54);
	ss2.str("");
	ss2 << "Enemy state: " << bossEnemy->GetState();
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 0), 1, 50, 50);
	ss2.str("");
	ss2 << "Guardian Health: " << guardian->gethealth();
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 0), 1, 50, 46);

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