
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only light.hpp

// TODO: this should probably be an interface and we would have
//       classes PointLight and AreaLight implementing it (maybe even SpotLight!)

#ifndef HAVE_LIGHT_H
#define HAVE_LIGHT_H

#include "glm.hpp"

using glm::vec3;

/**
 * Light represents a point light. The code should be self-explanatory
 */
class Light
{
public:
    Light();
    
    vec3 position = vec3(0,0,0);
    vec3 specularPower = vec3(1,1,1);
    vec3 diffusePower = vec3(1,1,1);
};

#endif
