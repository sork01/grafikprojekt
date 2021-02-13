
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only material.hpp

#ifndef HAVE_MATERIAL_H
#define HAVE_MATERIAL_H

#include "glm.hpp"
#include "texture.hpp"

using glm::vec3;

/**
 * Material is a structure for everything relevant to the material system
 * Every Intersection contains a Material instance, giving the material properties
 * of the intersected object at the point of intersection.
 */
class Material
{
public:
    /**
     * Create a new default material
     */
    Material();
    
    /**
     * Create a new material with a given color. The color is applied to the ambient and diffuse components
     */
    Material(vec3 color);
    virtual ~Material();
    
    /**
     * Index of refraction
     */
    float refraction = 1.0f;
    
    /**
     * Opacity/transparency. Range of each component is [0,1] where (0,0,0) indicates a fully transparent material
     */
    vec3 opacity = vec3(1.0f);
    
    /**
     * Specular (mirror-like) reflection. Range of each component is [0,1] where (1,1,1) indicates a perfect mirror
     */
    vec3 reflectivity = vec3(0.0f);
    
    /**
     * Metallic reflection mode. If true, any color reflected by this material should be heavily
     * tinted by the diffuse color of this material.
     */
    bool metallic = false;
    
    /**
     * Base Phong illumination specular color factors, or weights on texture colors if using a specular texture
     */
    vec3 specularFactors = vec3(0.01f);
    
    /**
     * Base Phong illumination diffuse color factors, or weights on texture colors if using a diffuse texture
     */
    vec3 diffuseFactors = vec3(0.5f);
    
    /**
     * Base Phong illumination ambient color factors, or weight on texture colors if using an ambient texture
     */
    vec3 ambientFactors = vec3(0.5f);
    
    /**
     * Specular texture, colors weighted by specularFactors
     */
    Texture* specularTexture = nullptr;
    
    /**
     * Diffuse texture, colors weighted by diffuseFactors
     */
    Texture* diffuseTexture = nullptr;
    
    /**
     * Ambient texture, colors weighted by ambientFactors
     */
    Texture* ambientTexture = nullptr;
    
    /**
     * Phong illumination shininess parameter
     */
    float shininess = 1.0f;
};

#endif
