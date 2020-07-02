#include "SceneObject.h"



SceneObject::SceneObject()
{
	glass = false;
	mirror = false;
	trans = 0;
}


SceneObject::~SceneObject()
{
}

bool SceneObject::isGlass()
{
	return glass;
}

void SceneObject::setGlass(bool g)
{
	glass = g;
}

bool SceneObject::isMirror()
{
	return mirror;
}

void SceneObject::setMirror(bool m)
{
	mirror = m;
}

void SceneObject::setTrans(float newT)
{
	trans = newT;
}

float SceneObject::getTrans()
{
	return trans;
}
