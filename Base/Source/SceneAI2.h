#ifndef SCENE_AI1_H
#define SCENE_AI1_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"
#include <string>
using std::string;
class Priest;
class Guardian;
class SceneAI2 : public SceneBase
{

public:
	SceneAI2();
	~SceneAI2();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	/************* GO ****************/
	void InitGO();
	void RenderGO(GameObject *go);
	GameObject* FetchGO();
	/*********************************/

	void UpdateMouse(double dt);
	void UpdateKeys(double dt);
	void UpdatePhysics(double dt);
	void UpdateFSM(double dt);

protected:

	float m_worldWidth;
	float m_worldHeight;

	float textureOffset[2];

	int m_objectCount;
	Priest* priest;
	Guardian* guardian;
};



#endif