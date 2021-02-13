
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only scene.cpp

#include "scene.hpp"

/**
 * Representation of a scene, code should be self-explanatory
 */

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::addObject(Object* obj)
{
    objects.push_back(obj);
}

void Scene::removeObject(Object* obj)
{
    objects.remove(obj);
}

const list<Object*> Scene::getObjects() const
{
    return objects;
}

void Scene::addLight(Light* light)
{
    lights.push_back(light);
}

void Scene::removeLight(Light* light)
{
    lights.remove(light);
}

const list<Light*> Scene::getLights() const
{
    return lights;
}
