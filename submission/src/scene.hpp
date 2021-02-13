
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only scene.hpp

#ifndef HAVE_SCENE_H
#define HAVE_SCENE_H

#include "camera.hpp"
#include "light.hpp"
#include "object.hpp"
#include <list>

using namespace std;

/**
 * Representation of a scene, code should be self-explanatory
 */
class Scene
{
public:
    Scene();
    // TODO: copy contructor for completeness
    ~Scene();
    
    void addObject(Object* o);
    void removeObject(Object* o);
    
    void addLight(Light* light);
    void removeLight(Light* light);
    
    const list<Object*> getObjects() const;
    const list<Light*> getLights() const;
    
    Camera camera;
    
    vec3 ambientLightPower = vec3(0.33f, 0.33f, 0.33f);

private:
    list<Object*> objects;
    list<Light*> lights;
};

#endif
