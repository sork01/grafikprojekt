
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only sphere.hpp

#ifndef HAVE_SPHERE_H
#define HAVE_SPHERE_H

#include "object.hpp"

using glm::vec3;

/**
 * Sphere represents a three-dimensional sphere defined by position and radius
 */
class Sphere : public Object
{
public:
    /**
     * Create a new sphere at the given position, of the given radius and using the given material
     */
    Sphere(vec3 position, float radius, Material material);
    ~Sphere();
    
    /**
     * Get a wireframe representation of the sphere
     */
    void getWireframe(vector<WireframeElement>& stuff);
    
    /**
     * Find the closest point of intersection between a given ray origin plus direction and the
     * sphere
     */
    bool getIntersection(vec3 from, vec3 dir, Intersection& is);
    
    /**
     * Remove any invisble object elements from intersection testing. Not actually applicable
     * to spheres
     */
    void cullInvisibleParts(vec3 cameraPosition);

private:
    vec3 position;
    float radius;
    Material material;
};

#endif
