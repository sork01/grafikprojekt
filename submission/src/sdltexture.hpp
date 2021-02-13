
// *neode.onsave* setgo gcc -I/usr/include/glm -I/usr/include/SDL2 -fsyntax-only sdltexture.hpp

#ifndef HAVE_SDLTEXTURE_H
#define HAVE_SDLTEXTURE_H

#include "texture.hpp"
#include "SDL.h"

using namespace std;
using glm::vec2;
using glm::vec3;

class SDLTexture : public Texture
{
public:
    /**
     * Load the given file
     */
    SDLTexture(string filename);
    // TODO: copy constructor
    // TODO: copy assignment operator
    // TODO: move constructor
    // TODO: move assignment operator
    ~SDLTexture();
    
    /**
     * Get the color at the specified texture coords, each texture coord being in the range [0,1]
     */
    vec3 getColorAt(vec2 coords);

private:
    /**
     * Image data
     */
    SDL_Surface* surface = nullptr;
    
    /**
     * Pixel access to image data
     */
    uint8_t* pixels;
    
    float width;
    float height;
};

#endif
