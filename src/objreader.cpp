
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only objreader.cpp

#include "objreader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

OBJReader::OBJReader()
{
    
}

OBJReader::~OBJReader()
{
    
}

Mesh* OBJReader::loadFile(string filename, OBJReader::VertexOrder order)
{
    ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
    return loadString(buffer.str(), order);
}

Mesh* OBJReader::loadString(string str, OBJReader::VertexOrder order)
{
    Mesh* result = new Mesh();
    istringstream data(str);
    
    string op, face_elem0, face_elem1, face_elem2;
    char c;
    
    float v0, v1, v2;
    
    int face_vertex, face_texture, face_normal;
    
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec3> faces;
    vector<glm::vec3> face_normals;
    
    while (data.good())
    {
        data >> op;
        
        if (data.fail())
        {
            data >> c;
            
            if (data.eof())
            {
                data.clear();
                break;
            }
        }
        
        if (op != "" && op.at(0) == '#')
        {
            // cout << "comment" << endl;
            while (data.good() && data.get(c) && c != '\n');
        }
        else if (op == "v")
        {
            // cout << "vertex" << endl;
            data >> v0 >> v1 >> v2;
            
            vertices.push_back(glm::vec3(v0, v1, v2));
        }
        else if (op == "vn")
        {
            // cout << "normal" << endl;
            data >> v0 >> v1 >> v2;
            
            normals.push_back(glm::vec3(v0, v1, v2));
        }
        else if (op == "f")
        {
            // cout << "face" << endl;
            
            face_elem0 = "";
            face_elem1 = "";
            face_elem2 = "";
            
            data >> face_elem0 >> face_elem1 >> face_elem2;
            
            string face_elements[3] = {face_elem0, face_elem1, face_elem2};
            int face_vertices[3] = {0,0,0};
            int face_norms[3] = {0,0,0};
            
            for (int i = 0; i < 3; ++i)
            {
                if (sscanf(face_elements[i].c_str(), "%d/%d/%d", &face_vertex, &face_texture, &face_normal) == 3)
                {
                    face_vertices[i] = face_vertex;
                    face_norms[i] = face_normal;
                }
                else if (sscanf(face_elements[i].c_str(), "%d//%d", &face_vertex, &face_normal) == 2)
                {
                    face_vertices[i] = face_vertex;
                    face_norms[i] = face_normal;
                }
                else if (sscanf(face_elements[i].c_str(), "%d", &face_vertex) == 1)
                {
                    face_vertices[i] = face_vertex;
                    face_norms[i] = -1;
                }
                else
                {
                    data.setstate(data.failbit);
                    break;
                }
            }
            
            if (data.good())
            {
                faces.push_back(glm::vec3(face_vertices[0], face_vertices[1], face_vertices[2]));
                face_normals.push_back(glm::vec3(face_norms[0], face_norms[1], face_norms[2]));
            }
        }
        else
        {
            // cout << "unknown: " << op << endl;
            while (data.good() && data.get(c) && c != '\n');
        }
        
        op = "";
    }
    
    if (data.fail())
    {
        cout << "objreader: failed" << endl;
    }
    else
    {
        for (uint i = 0; i < faces.size(); ++i)
        {
            glm::vec3 face = faces[i] - glm::vec3(1,1,1);
            glm::vec3 normal = face_normals[i] - glm::vec3(1,1,1);
            
            if (face.x < 0 || face.x >= vertices.size() || face.y < 0 || face.y >= vertices.size() || face.z < 0 || face.z >= vertices.size())
            {
                // error: vertex index out of range
                cout << "objreader: face vertex index out of range" << endl;
                break;
            }
            
            if (normal.x < 0 || normal.x >= normals.size() || normal.y < 0 || normal.y >= normals.size() || normal.z < 0 || normal.z >= normals.size())
            {
                // error: vertex index out of range
                cout << "objreader: face vertex normal index out of range" << endl;
                break;
            }
            
            // triangles.push_back(Triangle(vertices[face.x], vertices[face.y], vertices[face.z], glm::vec3(1,1,1)));
            
            // vec3 e1 = vertices[face.y] - vertices[face.x];
            // vec3 e2 = vertices[face.z] - vertices[face.x];
            
            // vec3 normal = glm::cross(e1, e2);
            
            // yes, this is pretty terrible
            switch (order)
            {
                case ABC:
                    result->pushVertex(vertices[face.x], normals[normal.x]);
                    result->pushVertex(vertices[face.y], normals[normal.y]);
                    result->pushVertex(vertices[face.z], normals[normal.z]);
                    break;
                case ACB:
                    result->pushVertex(vertices[face.x], normals[normal.x]);
                    result->pushVertex(vertices[face.z], normals[normal.z]);
                    result->pushVertex(vertices[face.y], normals[normal.y]);
                    break;
                case BAC:
                    result->pushVertex(vertices[face.y], normals[normal.y]);
                    result->pushVertex(vertices[face.x], normals[normal.x]);
                    result->pushVertex(vertices[face.z], normals[normal.z]);
                    break;
                case BCA:
                    result->pushVertex(vertices[face.y], normals[normal.y]);
                    result->pushVertex(vertices[face.z], normals[normal.z]);
                    result->pushVertex(vertices[face.x], normals[normal.x]);
                    break;
                case CAB:
                    result->pushVertex(vertices[face.z], normals[normal.z]);
                    result->pushVertex(vertices[face.x], normals[normal.x]);
                    result->pushVertex(vertices[face.y], normals[normal.y]);
                    break;
                case CBA:
                    result->pushVertex(vertices[face.z], normals[normal.z]);
                    result->pushVertex(vertices[face.y], normals[normal.y]);
                    result->pushVertex(vertices[face.x], normals[normal.x]);
                    break;
                default:
                    break;
            }
            
            result->commitTriangle(Material());
        }
    }
    
    return result;
}
