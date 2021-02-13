
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only sphere.hpp

#ifndef HAVE_SPHERE_H
#define HAVE_SPHERE_H

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "object.hpp"

using glm::vec3;
using glm::quat;

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
    bool getIntersection(vec3 from, vec3 dir, Intersection& is, bool enableCulling);
    
    /**
     * Remove any invisble object elements from intersection testing. Not actually applicable
     * to spheres
     */
    void cullInvisibleParts(vec3 cameraPosition);
    
    void setRadius(float radius);
    
    void setPosition(vec3 position);
    void move(vec3 amount);
    void rotate(vec3 axis, float radians);
    void scale(vec3 factors);
    void scale(float factor);
    void flipX();
    void flipY();
    void flipZ();

private:
    vec3 m_position;
    float m_radius;
    Material m_material;
    quat m_rotation;
};

#endif
