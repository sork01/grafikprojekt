
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only -lstdc++ object.hpp

#ifndef HAVE_OBJECT_H
#define HAVE_OBJECT_H

#include "glm.hpp"
#include "wireframeelement.hpp"
#include "intersection.hpp"
#include <vector>

using namespace std;
using glm::vec3;

/**
 * Object is an interface for 3D objects
 */
class Object
{
public:
    virtual ~Object(){}
    
    /**
     * Get a wireframe representation of the object
     */
    virtual void getWireframe(vector<WireframeElement>& stuff) = 0;
    
    /**
     * Find the closest intersection of a ray and the object with the ray starting at a given
     * origin and having a given direction
     */
    virtual bool getIntersection(vec3 from, vec3 dir, Intersection& is, bool enableCulling) = 0;
    
    /**
     * Remove invisble object parts from intersection tests
     */
    virtual void cullInvisibleParts(vec3 cameraPosition) = 0;
    
    virtual void setPosition(vec3 position) = 0;
    virtual void move(vec3 amount) = 0;
    virtual void rotate(vec3 axis, float radians) = 0;
    virtual void scale(vec3 factors) = 0;
    
    virtual void scale(float factor) = 0;
    
    virtual void flipX() = 0;
    virtual void flipY() = 0;
    virtual void flipZ() = 0;
};

#endif
