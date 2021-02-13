
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
    WireframeRenderer();
    /**
     * Render a given scene by writing an image of the given width and height to the given output
     */
    void renderScene(Scene& s, int width, int height, PixelSurface& output);
    list<Renderer::Option> getOptions();
    void setOptions(list<Renderer::Option> opts);
    void setOption(Option opt);
    
private:
    bool opt_ShowVertexNormals = false;
    bool opt_ShowCentroidNormals = false;
};

#endif
