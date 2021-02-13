
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only intersection.hpp

#ifndef HAVE_INTERSECTION_H
#define HAVE_INTERSECTION_H

#include "material.hpp"

using glm::vec2;

/**
 * Intersection is a structure for data relevant to a ray-object intersection
 */
struct Intersection
{
    /**
     * The world coordinates of the point of intersection
     */
    vec3 position;
    
    /**
     * The normal vector of the object at the point of intersection
     */
    vec3 normal;
    
    /**
     * The texture coordinates of the object at the point of intersection
     */
    vec2 textureCoords;
    
    /**
     * The distance from the ray origin to the point of intersection
     */
    float distance;
    
    /**
     * The material of the object at the point of intersection
     */
    Material material;
};

#endif
