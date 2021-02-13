
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only material.cpp

#include "material.hpp"
#include <iostream>

using namespace std;
using glm::vec3;

Material::Material()
{
}

// apply given color to both diffuse and ambient
Material::Material(vec3 color) : diffuseFactors(color), ambientFactors(color)
{
}

Material::~Material()
{   
}
