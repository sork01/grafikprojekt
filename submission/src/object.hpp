
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
    virtual bool getIntersection(vec3 from, vec3 dir, Intersection& is) = 0;
    
    /**
     * Remove invisble object parts from intersection tests
     */
    virtual void cullInvisibleParts(vec3 cameraPosition) = 0;
};

#endif
