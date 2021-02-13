
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only wireframerenderer.hpp

#ifndef HAVE_WIREFRAMERENDERER_H
#define HAVE_WIREFRAMERENDERER_H

#include "renderer.hpp"

/**
 * WireframeRenderer implements a Renderer that only draws wireframe representations of objects
 */
class WireframeRenderer : public Renderer
{
public:
    /**
     * Render a given scene by writing an image of the given width and height to the given output
     */
    void renderScene(Scene& s, int width, int height, PixelSurface& output);
};

#endif
