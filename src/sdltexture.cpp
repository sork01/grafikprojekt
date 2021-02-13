
// *neode.onsave* setgo gcc -I/usr/include/glm -I/usr/include/SDL2 -fsyntax-only sdltexture.cpp

#include "sdltexture.hpp"
#include "SDL_image.h"

/**
 * Load the given file
 */
SDLTexture::SDLTexture(string filename)
{
    surface = IMG_Load(filename.c_str());
    
    // harsh error handling
    if (!surface)
    {
        printf("%p %s\n", surface, IMG_GetError());
        exit(0);
    }
    
    // -1 so that the pixel data can be accessed in the form data[round(c * width)] for c in [0,1]
    width = ((float)surface->w) - 1.0f;
    height = ((float)surface->h) - 1.0f;
    
    pixels = (uint8_t*) surface->pixels;
    numbytes = surface->w * surface->h * 4;
}

/**
 * Cleanup
 */
SDLTexture::~SDLTexture()
{
    if (surface != nullptr)
    {
        SDL_FreeSurface(surface);
    }
}

/**
 * Get the color at the specified texture coords, each texture coord being in the range [0,1]
 * This only works for images in the RGBA8888 format currently
 */
vec3 SDLTexture::getColorAt(vec2 coords)
{
    int x, y, offset;
    
    x = round(coords.x * width);
    y = round(coords.y * height);
    
    offset = (y * surface->pitch + 4*x) % numbytes;
    
    vec3 result((float)pixels[offset] / 255.0f, (float)pixels[offset+1] / 255.0f, (float)pixels[offset+2] / 255.0f);
    
    return result;
}
