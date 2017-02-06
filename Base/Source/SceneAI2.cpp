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
#include "NodeManager.h"
#include "Node.h"
#include "AStarAlgorithm.h"

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
	nodemanager = NodeManager::GetInstance();

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
	bossEnemy->health = 50;
	bossEnemy->SetIsLeader(true);
	GameObjectManager::GetInstance()->m_goList.push_back(bossEnemy);

	manaArea.Set(20, 20, 1);
	manaPos.Set(m_worldWidth / 2, 80, 0);
	priest->manaPos = manaPos;

	int rdm = Math::RandIntMinMax(0, GRID_COLS - 1);
	Node *start; Node *goal;
	for (int i = 0; i < GRID_COLS; ++i) {
		for (int j = 0; j < GRID_ROWS; ++j) {
			Node* node = new Node();
			if (i == rdm && j == 0) {
				node->grid = new Grid(Vector3((float)((GRID_SIZE >> 1) + GRID_SIZE * i), (float)((GRID_SIZE >> 1) + GRID_SIZE * j), 0), Vector3(GRID_SIZE, GRID_SIZE, 1), Grid::START);
				start = node;
			}
			else if (i == GRID_COLS - 1 && j == ((GRID_ROWS + 1) >> 1)) {
				node->grid = new Grid(Vector3((float)((GRID_SIZE >> 1) + GRID_SIZE * i), (float)((GRID_SIZE >> 1) + GRID_SIZE * j), 0), Vector3(GRID_SIZE, GRID_SIZE, 1), Grid::END);
				goal = node;
			}
			else {
				int rdm2 = Math::RandIntMinMax(0, 10);
				if (rdm2 < 8)
					node->grid = new Grid(Vector3((float)((GRID_SIZE >> 1) + GRID_SIZE * i), (float)((GRID_SIZE >> 1) + GRID_SIZE * j), 0), Vector3(GRID_SIZE, GRID_SIZE, 1), Grid::EMPTY);
				else
					node->grid = new Grid(Vector3((float)((GRID_SIZE >> 1) + GRID_SIZE * i), (float)((GRID_SIZE >> 1) + GRID_SIZE * j), 0), Vector3(GRID_SIZE, GRID_SIZE, 1), Grid::WALL);
			}
			nodemanager->Init(i, j, node);
		}
	}
	if (start && goal) {
		NodeManager::GetInstance()->CalculateFGHCost(start, goal);
	}

	AStarAlgorithm(start, goal);
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
		guardian->health = 0;
	}
	if (KeyboardController::GetInstance()->IsKeyDown('S')) {
		priest->health = 0;
	}
	if (KeyboardController::GetInstance()->IsKeyDown('A')) {

	}
	if (KeyboardController::GetInstance()->IsKeyDown('D')) {

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
	for (size_t i = 0; i < GRID_ROWS; ++i) {
		for (size_t j = 0; j < GRID_COLS; ++j) {

		modelStack.PushMatrix();
		modelStack.Translate(nodemanager->theNode[j][i]->grid->pos.x, nodemanager->theNode[j][i]->grid->pos.y, nodemanager->theNode[j][i]->grid->pos.z);
		modelStack.Scale(nodemanager->theNode[j][i]->grid->scale.x, nodemanager->theNode[j][i]->grid->scale.y, nodemanager->theNode[j][i]->grid->scale.z);

		/*if (nodemanager->theNode[j][i]->parent)
			RenderMesh(meshList[GRID_PATH], false);
		else if (nodemanager->theNode[j][i]->grid->type == Grid::EMPTY)
			RenderMesh(meshList[GRID_EMPTY], false);*/
		if (nodemanager->theNode[j][i]->grid->type == Grid::START)
			RenderMesh(meshList[GRID_START], false);
		else if (nodemanager->theNode[j][i]->grid->type == Grid::END)
			RenderMesh(meshList[GRID_END], false);
		else if (nodemanager->theNode[j][i]->parent)
			RenderMesh(meshList[GRID_PATH], false);
		else if (nodemanager->theNode[j][i]->grid->type == Grid::WALL)
			RenderMesh(meshList[GRID_WALL], false);
		else if (nodemanager->theNode[j][i]->visited)
			RenderMesh(meshList[GRID_EMPTY], false);

		

		std::ostringstream op;
		op.str("");
		op << nodemanager->theNode[j][i]->F;
		modelStack.Scale(0.5f, 0.5f, 1);
		RenderText(meshList[GEO_TEXT],op.str(), Color(0, 0.4f, 0.4f), 0.4f);
		modelStack.PopMatrix();
		}
	}

	modelStack.PushMatrix();
	modelStack.Translate(manaPos.x, manaPos.y, manaPos.z);
	modelStack.Scale(manaArea.x, manaArea.y, manaArea.z);
	RenderMesh(meshList[MANA_AREA], false);
	modelStack.PopMatrix();

	float yCoordinates = 58.f;
	std::ostringstream ss;
	for (size_t i = 0; i < MessageBoard::GetInstance()->GetList().size(); ++i)
	{
		ss.str("");
		ss << i << ": Message Board receives \"" << MessageBoard::GetInstance()->GetList()[i] << "\"";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 1, yCoordinates - i * 1.5f);
	}
	//
	// 
	// << " \tHealth: " << bossEnemy->health
	std::ostringstream ss2;
	ss2.str("");
	ss2 << "Priest state: " << priest->GetState() ;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 0), 1, 20, 16);
	ss2.str("");														 
	ss2 << "Guardian state: " << guardian->GetState();
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 0), 1, 20, 14);
	ss2.str("");														 
	ss2 << "Enemy state: " << bossEnemy->GetState();
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 0), 1, 20, 12);


	ss2.str("");
	ss2 << "Priest Health: " << priest->health << " \tMana: " << priest->mana;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 0), 1, 50, 16);
	ss2.str("");
	ss2 << "Guardian Health " << guardian->health;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 0), 1, 50, 14);
	ss2.str("");
	ss2 << "Enemy Health: " << bossEnemy->health;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 0), 1, 50, 12);
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