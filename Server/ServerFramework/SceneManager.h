#pragma once
#include "Scene.h"
#include "MyInclude.h"
#include "MyDefine.h"
class SceneManager
{
private:
	Scene * pSceneList[MAX_SCENE];
	int preIndex;
public:
	SceneManager();
	~SceneManager();
public:
	void AddScene(int index, Scene* scene);
	void MoveScene(int index);
public:
	Scene * GetScene(){return pSceneList[preIndex];}

private:
	// 복사생성과 복사대입이 되지 않도록 방지.
	SceneManager(const SceneManager&);
	SceneManager& operator=(const SceneManager&);
};

