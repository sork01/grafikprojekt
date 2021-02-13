
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only stlreader.hpp

#ifndef HAVE_STLREADER_H
#define HAVE_STLREADER_H

#include "mesh.hpp"

class STLReader
{
public:
    STLReader();
    ~STLReader();
    Mesh* loadStl(string filename);
};

#endif
