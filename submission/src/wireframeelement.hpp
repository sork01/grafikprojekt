
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only wireframeelement.hpp

#ifndef HAVE_WIREFRAMEELEMENT_H
#define HAVE_WIREFRAMEELEMENT_H

#include <vector>
#include "glm.hpp"

using namespace std;
using glm::vec3;

/**
 * A WireframeElement is a collection of points (i.e vertices) with normals (i.e vertex normals)
 * along with an optional centroid point and centroid normal plus an indicator for how what type
 * of lines (wires) should be drawn between the points
 */
class WireframeElement
{
public:
    /**
     * The types of lines that can be used to connect points
     */
    enum Connector {Linear};
    
    vector<vec3> points;
    vector<vec3> normals;
    bool hasCentroid = false;
    vec3 centroid;
    vec3 centroidNormal;
    Connector connector = Linear;
};

#endif
