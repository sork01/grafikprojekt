// The first 80 bytes contains the header for the model (modelname or author).
// The next 32 bits contains the number of total triangles in the model.
// For every triangle there are 3*4 bytes containing the normal vectors of the triangle, 
// 3*(3*4 bytes information about the position of the vertex) and lastly there is 2 bytes of
// attribute byte count which is 0 in the standard format.

#include <iostream>
#include <fstream>
#include "glm.hpp"
#include "mesh.hpp"
#include "stlreader.hpp"
using namespace std;
using glm::vec3;

STLReader::STLReader()
{
    
}

STLReader::~STLReader()
{
    
}

Mesh* STLReader::loadStl(string filename)
{
  streampos size;
  streampos posv1;
  streampos posv2;
  streampos posv3;
  char * memblock;
  Mesh* result = new Mesh(); 

  ifstream file (filename, ios::in|ios::binary|ios::ate);
  if (file.is_open())
  {
    size = file.tellg();
    memblock = new char [size];
    // start from beginning of file
    file.seekg (0, ios::beg);
    // ignore the header
    file.ignore(80);
    uint32_t no_triangles;
    // get number of triangles 
    file.read(reinterpret_cast<char *>(&no_triangles), sizeof(no_triangles));
    printf("number of triangles: %d\n", no_triangles);
    // for every triangle in file
    for(uint i = 0; i < no_triangles; ++i)
    {
        float x;
        float y;
        float z;
        vec3 v1; 
        vec3 v2;
        vec3 v3; 
        // ignore the normals
        file.ignore(12);
        // first vertex
        file.read(reinterpret_cast<char *>(&x), 4);
        file.read(reinterpret_cast<char *>(&y), 4);
        file.read(reinterpret_cast<char *>(&z), 4);
        v1 = vec3(x,y,z);
        // second vertex    
        file.read(reinterpret_cast<char *>(&x), 4);
        file.read(reinterpret_cast<char *>(&y), 4);
        file.read(reinterpret_cast<char *>(&z), 4);
        v2 = vec3(x,y,z);
        // third vertex
        file.read(reinterpret_cast<char *>(&x), 4);
        file.read(reinterpret_cast<char *>(&y), 4);
        file.read(reinterpret_cast<char *>(&z), 4);
        v3 = vec3(x,y,z);
        // ignore attribute 
        file.ignore(2);
        // compute normals
        vec3 edge1 = v2 - v1;
        vec3 edge2 = v3 - v1;
        vec3 v1normal = glm::cross(edge1, edge2);
        vec3 v2normal = glm::cross(edge1, edge2);
        vec3 v3normal = glm::cross(edge1, edge2);
        // push vertex to mesh
        result->pushVertex(v1, v1normal);
        result->pushVertex(v2, v2normal);
        result->pushVertex(v3, v3normal);
        result->commitTriangle(Material());
        
        
    }
    file.close();

    delete[] memblock;
    return result;
  }
  else 
  {
  printf("Unable to open file");
  }
  return 0;
}