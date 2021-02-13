
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only renderer.hpp

#ifndef HAVE_RENDERER_H
#define HAVE_RENDERER_H

#include "pixelsurface.hpp"
#include "scene.hpp"
#include <cstdint>

/**
 * Renderer is an abstract class representing a system capable of producing
 * a two-dimensional image.
 */
class Renderer
{
public:
    /**
     * Render a scene.
     * @param s Scene to render
     * @param width Width of output image (TODO: grab these from PixelSurface.getDimensions?)
     * @param height Height of output image
     * @param pixels Pixel surface for output
     */
    virtual void renderScene(Scene& s, int width, int height, PixelSurface& output) = 0;
    virtual ~Renderer(){}
};

#endif
