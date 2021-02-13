
// *neode.onsave* setgo gcc -fsyntax-only pixelsurface.hpp

#ifndef HAVE_PIXELSURFACE_H
#define HAVE_PIXELSURFACE_H

#include <cstdint>

/**
 * PixelSurface is an interface that allows write-only access to some kind
 * of RGB pixel array.
 */
class PixelSurface
{
public:
    /**
     * Set the RGB color of the pixel located at (x, y)
     * @param x X coordinate of pixel to set
     * @param y Y coordinate of pixel to set
     * @param r Red component
     * @param g Green component
     * @param b Blue component
     */
    virtual void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) = 0;
    // get dimensions?
    
    virtual ~PixelSurface(){}
};

#endif
