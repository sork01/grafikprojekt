 
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only wireframerenderer.cpp

#include "wireframerenderer.hpp"
#include "wireframeelement.hpp"

// TODO: add way to toggle rendering of vertex normals and surface normals

using glm::vec3;
using glm::vec2;
void Linedraw(int x1, int y1, int x2, int y2, vec3 color, PixelSurface& output);

// TODO: remove this macro now that bounds checking in done elsewhere
#define DRAW(x,y,r,g,b) \
if (x >= 0 && x < width && y >= 0 && y < height)\
{\
    output.setPixel(x, y, r, g, b);\
}

/**
 * Render a given scene by writing an image of the given width and height to the given output
 */
void WireframeRenderer::renderScene(Scene& s, int width, int height, PixelSurface& output)
{
    vector<WireframeElement> stuff;
    
    for (Object* obj : s.getObjects())
    {
        // collect all wireframe elements
        obj->getWireframe(stuff);
    }
    
    float F = s.camera.getFocalLength();
    
    for (WireframeElement e : stuff)
    {
        for (uint i = 0; i < e.points.size(); ++i)
        {
            // connect each pair of points
            uint j = (i + 1) % e.points.size();
            
            vec3 u = s.camera.transformPoint(e.points[i]);
            vec3 v = s.camera.transformPoint(e.points[j]);
            
            if (u.z < 1.0f || v.z < 1.0f)
            {
                break;
            }
            
            int x1 = F*u.x/u.z + width/2;
            int y1 = F*u.y/u.z + height/2;
            
            int x2 = F*v.x/v.z + width/2;
            int y2 = F*v.y/v.z + height/2;
            
            Linedraw(x1, y1, x2, y2, vec3(200, 200, 200), output);
            
            vec3 n = s.camera.transformPoint(e.points[i] + (glm::normalize(e.normals[i]) / 5.0f));
            
            // draw the vertex normals
            int x3 = F*n.x/n.z + width/2;
            int y3 = F*n.y/n.z + height/2;
            
            Linedraw(x1, y1, x3, y3, vec3(220, 220, 60), output);
            
            DRAW(x1, y1, 255, 255, 255)
            DRAW(x1 - 1, y1, 255, 255, 255)
            DRAW(x1 + 1, y1, 255, 255, 255)
            DRAW(x1, y1 - 1, 255, 255, 255)
            DRAW(x1, y1 + 1, 255, 255, 255)
            
            // draw the centroid and centroid normal
            if (e.hasCentroid)
            {
                vec3 c = s.camera.transformPoint(e.centroid);
                
                int cx = F*c.x/c.z + width/2;
                int cy = F*c.y/c.z + height/2;
                
                DRAW(cx, cy, 255, 0, 0);
                DRAW(cx-1, cy, 255, 0, 0);
                DRAW(cx+1, cy, 255, 0, 0);
                DRAW(cx, cy-1, 255, 0, 0);
                DRAW(cx, cy+1, 255, 0, 0);
                
                vec3 cn = s.camera.transformPoint(e.centroid + (glm::normalize(e.centroidNormal) / 5.0f));
                
                int cnx = F*cn.x/cn.z + width/2;
                int cny = F*cn.y/cn.z + height/2;
                
                Linedraw(cx, cy, cnx, cny, vec3(255, 0, 0), output);
            }
        }
    }
    
    // draw the lights
    for (Light* light : s.getLights())
    {
        vec3 mid = light->position;
        vec3 left = s.camera.transformPoint(mid + vec3(-0.3f, 0, 0));
        vec3 right = s.camera.transformPoint(mid + vec3(0.3f, 0, 0));
        vec3 top = s.camera.transformPoint(mid + vec3(0, -0.3f, 0));
        vec3 bot = s.camera.transformPoint(mid + vec3(0, 0.3f, 0));
        vec3 front = s.camera.transformPoint(mid + vec3(0, 0, -0.3f));
        vec3 back = s.camera.transformPoint(mid + vec3(0, 0, 0.3f));
        
        vec3 c1 = s.camera.transformPoint(mid + vec3(-0.2f, -0.2f, -0.2f));
        vec3 c2 = s.camera.transformPoint(mid + vec3(0.2f, -0.2f, -0.2f));
        vec3 c3 = s.camera.transformPoint(mid + vec3(-0.2f, -0.2f, 0.2f));
        vec3 c4 = s.camera.transformPoint(mid + vec3(0.2f, -0.2f, 0.2f));
        vec3 c5 = s.camera.transformPoint(mid + vec3(-0.2f, 0.2f, -0.2f));
        vec3 c6 = s.camera.transformPoint(mid + vec3(0.2f, 0.2f, -0.2f));
        vec3 c7 = s.camera.transformPoint(mid + vec3(-0.2f, 0.2f, 0.2f));
        vec3 c8 = s.camera.transformPoint(mid + vec3(0.2f, 0.2f, 0.2f));
        
        if (left.z < 1.0f || right.z < 1.0f || top.z < 1.0f || bot.z < 1.0f || front.z < 1.0f || back.z < 1.0f)
        {
            break;
        }
        
        int x1 = F*left.x/left.z + width/2;
        int y1 = F*left.y/left.z + height/2;
        
        int x2 = F*right.x/right.z + width/2;
        int y2 = F*right.y/right.z + height/2;
        
        Linedraw(x1, y1, x2, y2, vec3(255, 255, 0), output);
        
        x1 = F*top.x/top.z + width/2;
        y1 = F*top.y/top.z + height/2;
        
        x2 = F*bot.x/bot.z + width/2;
        y2 = F*bot.y/bot.z + height/2;
        
        Linedraw(x1, y1, x2, y2, vec3(255, 255, 0), output);
        
        x1 = F*front.x/front.z + width/2;
        y1 = F*front.y/front.z + height/2;
        
        x2 = F*back.x/back.z + width/2;
        y2 = F*back.y/back.z + height/2;
        
        Linedraw(x1, y1, x2, y2, vec3(255, 255, 0), output);
        
        x1 = F*c1.x/c1.z + width/2;
        y1 = F*c1.y/c1.z + height/2;
        
        x2 = F*c8.x/c8.z + width/2;
        y2 = F*c8.y/c8.z + height/2;
        
        Linedraw(x1, y1, x2, y2, vec3(255, 255, 0), output);
        
        x1 = F*c2.x/c2.z + width/2;
        y1 = F*c2.y/c2.z + height/2;
        
        x2 = F*c7.x/c7.z + width/2;
        y2 = F*c7.y/c7.z + height/2;
        
        Linedraw(x1, y1, x2, y2, vec3(255, 255, 0), output);
        
        x1 = F*c3.x/c3.z + width/2;
        y1 = F*c3.y/c3.z + height/2;
        
        x2 = F*c6.x/c6.z + width/2;
        y2 = F*c6.y/c6.z + height/2;
        
        Linedraw(x1, y1, x2, y2, vec3(255, 255, 0), output);
        
        x1 = F*c4.x/c4.z + width/2;
        y1 = F*c4.y/c4.z + height/2;
        
        x2 = F*c5.x/c5.z + width/2;
        y2 = F*c5.y/c5.z + height/2;
        
        Linedraw(x1, y1, x2, y2, vec3(255, 255, 0), output);
    }
}

/**
 * Simple line drawing algorithm using linear interpolation
 */
void Linedraw(int x1, int y1, int x2, int y2, vec3 color, PixelSurface& output)
{
    int dy = abs(y2 - y1);
    int dx = abs(x2 - x1);
    float no_steps = max(1.0f, float(glm::max(dy, dx)));
    
    float y_step = (y2 - y1)/no_steps;
    float x_step = (x2 - x1)/no_steps;
    
    float x = x1;
    float y = y1;
    
    for(int i = 0; i < no_steps; ++i )
    {
        output.setPixel(x, y, color.x, color.y, color.z);
        
        x += x_step;
        y += y_step;
    }
}
