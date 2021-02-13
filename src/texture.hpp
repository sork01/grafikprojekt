
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only texture.hpp

#ifndef HAVE_TEXTURE_H
#define HAVE_TEXTURE_H

#include "glm.hpp"
#include <string>

using namespace std;
using glm::vec2;
using glm::vec3;

/**
 * Texture interface
 */
class Texture
{
public:
    virtual ~Texture(){}
    
    /**
     * Get the color at the specified texture coords, each texture coord being in the range [0,1]
     */
    virtual vec3 getColorAt(vec2 coords) = 0;
};

#endif
