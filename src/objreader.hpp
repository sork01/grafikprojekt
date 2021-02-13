
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only objreader.hpp

#include "mesh.hpp"

/**
 * OBJReader reads .OBJ files and creates Mesh instances
 */
class OBJReader
{
public:
    /**
     * VertexOrder represents the six possible orderings of placing the vertices read from
     * the .OBJ file into triangle vertices in the result mesh. This is useful for files with
     * vertex orderings that produce incorrect surface normals.
     */
    enum VertexOrder {ABC, ACB, BAC, BCA, CAB, CBA};
    OBJReader();
    ~OBJReader();
    
    Mesh* loadFile(string filename, OBJReader::VertexOrder order=ABC);
    Mesh* loadString(string data, OBJReader::VertexOrder order=ABC);
};
