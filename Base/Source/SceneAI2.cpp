#include "SceneAI2.h"

// Managers
#include "Application.h"
#include "MeshBuilder.h"
#include "KeyboardController.h"
#include "GO_List.h"

// Libs
#include "GL\glew.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>

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
}


GameObject* SceneAI2::FetchGO()
{
	for (std::vector<GameObject *>::iterator it = GameObjectList::GetInstance()->m_goList.begin(); it != GameObjectList::GetInstance()->m_goList.end(); ++it)
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
		GameObjectList::GetInstance()->m_goList.push_back(new GameObject(GameObject::GO_NONE));
	}

	GameObject *go = *(GameObjectList::GetInstance()->m_goList.end() - 1);
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

	}
	if (KeyboardController::GetInstance()->IsKeyDown('S')) {

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
	for (std::vector<GameObject *>::iterator it = GameObjectList::GetInstance()->m_goList.begin(); it != GameObjectList::GetInstance()->m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->active)
			continue;

		go->pos += go->vel * (float)dt;
		float radius = go->scale.x;

		//Exercise 8a: handle collision between GO_BALL and GO_BALL using velocity swap
		for (std::vector<GameObject *>::iterator ho = it + 1; ho != GameObjectList::GetInstance()->m_goList.end(); ++ho)
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
	fps = 1.f / (float)dt;

	UpdateKeys(dt);
	UpdateMouse(dt);
	UpdatePhysics(dt);
}

void SceneAI2::RenderGO(GameObject *go)
{
	modelStack.PushMatrix();
	modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
	float angle = -90.f + Math::RadianToDegree(atan2(go->normal.y, go->normal.x));
	modelStack.Rotate(angle, 0, 0, 1);
	modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
	modelStack.PopMatrix();
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

	for (std::vector<GameObject *>::iterator it = GameObjectList::GetInstance()->m_goList.begin(); it != GameObjectList::GetInstance()->m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}

	std::ostringstream ss;
	ss.precision(1);
	ss << "HI";
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 10, 3);

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	RenderMesh(meshList[GRID], false);
	modelStack.PopMatrix();

	RenderMesh(meshList[GEO_AXES], false);
}

void SceneAI2::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (GameObjectList::GetInstance()->m_goList.size() > 0)
	{
		GameObject *go = GameObjectList::GetInstance()->m_goList.back();
		delete go;
		GameObjectList::GetInstance()->m_goList.pop_back();
	}
}