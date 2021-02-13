
// *neode.onsave* setgo gcc -I/usr/include/glm -I/usr/include/SDL2 -fsyntax-only mesh.cpp

#include "tribox3.h"
#include "mesh.hpp"
#include "sdltexture.hpp"
#include <iostream>
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

using namespace std;
using namespace glm;

/**
 * Create a new empty mesh
 */
Mesh::Mesh()
{
    m_mainbox = new BoundingBox();
}

Mesh::~Mesh()
{
    delete m_mainbox;
}

/**
 * Retrieve the wireframe elements of the mesh
 */
void Mesh::getWireframe(vector<WireframeElement>& stuff)
{
    for (Triangle t : m_triangles)
    {
        WireframeElement e;
        
        // add the vertices
        e.points.push_back(m_position + t.vertex[0].position);
        e.points.push_back(m_position + t.vertex[1].position);
        e.points.push_back(m_position + t.vertex[2].position);
        
        // add the vertex normals
        e.normals.push_back(t.vertex[0].normal);
        e.normals.push_back(t.vertex[1].normal);
        e.normals.push_back(t.vertex[2].normal);
        
        // add the centroid along with the surface normal
        e.hasCentroid = true;
        
        e.centroid = vec3(
            (1.0f/3.0f)*(t.vertex[0].position.x + t.vertex[1].position.x + t.vertex[2].position.x),
            (1.0f/3.0f)*(t.vertex[0].position.y + t.vertex[1].position.y + t.vertex[2].position.y),
            (1.0f/3.0f)*(t.vertex[0].position.z + t.vertex[1].position.z + t.vertex[2].position.z));
        
        e.centroid += m_position;
        
        e.centroidNormal = t.geometricNormal;
        
        stuff.push_back(e);
    }
    
    // bboxwire(stuff, m_mainbox);
}

/**
 * Compute the barycentric coordinates of a point p given triangle vertices a, b and c
 * Source: of https://gamedev.stackexchange.com/a/23745
 */
vec3 Mesh::computeBarycentricCoords(vec3 p, vec3 a, vec3 b, vec3 c)
{
    vec3 v0 = b - a;
    vec3 v1 = c - a;
    vec3 v2 = p - a;
    
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0 - v - w;
    
    return vec3(u, v, w);
}

/**
 * Real intersection tester that applies the BVH optimization
 */
bool Mesh::getIntersection(vec3 start, vec3 dir, vec3 dir_inv, Intersection& is, bool enableCulling, BoundingBox* box)
{
    bool result = false;
    float min_distance = std::numeric_limits<float>::max();
    
    if (!rayIntersectsBox(start, dir_inv, box))
    {
        return false;
    }
    
    // if box has subboxes, any triangle contained in current box is guaranteed to be contained
    // in atleast one of the subboxes. so check subboxes instead of checking current box, because
    // the ray hopefully only intersects one of the subboxes
    if (box->hasSubs)
    {
        Intersection subis;
        
        if (box->sub[0] != nullptr && getIntersection(start, dir, dir_inv, subis, enableCulling, box->sub[0]))
        {
            is = subis;
            result = true;
            min_distance = is.distance;
        }
        
        // for the second subbox we have to care about the min_distance, since there is a possibility
        // that an intersection has been found in the first subbox
        if (box->sub[1] != nullptr && getIntersection(start, dir, dir_inv, subis, enableCulling, box->sub[1]) && subis.distance < min_distance)
        {
            is = subis;
            result = true;
        }
    }
    else // no subboxes, test triangles in current box
    {
        Triangle* tf;
        
        for (Triangle* t : box->triangles)
        {
            if (enableCulling && t->hidden)
            {
                // triangle is culled
                continue;
            }
            
            // source: labs
            vec3 e1 = t->vertex[1].position - t->vertex[0].position;
            vec3 e2 = t->vertex[2].position - t->vertex[0].position;
            vec3 b = start - (m_position + t->vertex[0].position);
            mat3 A(-dir, e1, e2);
            vec3 x = glm::inverse(A) * b;
            
            if (x.x >= 0 && x.y >= 0 && x.z >= 0 && x.y + x.z <= 1)
            {
                vec3 position = start + x.x*dir;
                float distance = glm::length(x.x*dir);
                
                if (distance < min_distance)
                {
                    min_distance = distance;
                    
                    result = true;
                    
                    is.distance = distance;
                    is.position = position;
                    
                    tf = t;
                }
            }
        }
        
        if (result)
        {
            vec3 bc = computeBarycentricCoords(is.position,
                m_position + tf->vertex[0].position,
                m_position + tf->vertex[1].position,
                m_position + tf->vertex[2].position);
            
            // phong interpolation
            is.normal = bc.x*tf->vertex[0].normal + bc.y*tf->vertex[1].normal + bc.z*tf->vertex[2].normal;
            
            is.material = tf->material;
            
            is.textureCoords = bc.x*tf->vertex[0].textureCoords + bc.y*tf->vertex[1].textureCoords + bc.z*tf->vertex[2].textureCoords;
        }
    }
    
    return result;
}

/**
 * Find the closest intersection given a ray origin and a direction
 */
bool Mesh::getIntersection(vec3 start, vec3 dir, Intersection& is, bool enableCulling)
{
    // computing 1/dir was suggested in source of ray-box intersection tests
    vec3 dir_inv(1.0f/dir.x, 1.0f/dir.y, 1.0f/dir.z);
    return getIntersection(start, dir, dir_inv, is, enableCulling, m_mainbox);
}

/**
 * Remove invisible triangles from intersection testing
 */
void Mesh::cullInvisibleParts(vec3 cameraPosition)
{
    for (Triangle& t : m_triangles)
    {
        // remove triangles where the angle between the surface normal and a vector to the camera is greater than 90 degrees
        if (glm::dot(glm::normalize(cameraPosition - (m_position + t.vertex[0].position)), t.geometricNormal) < 0.0f)
        {
            t.hidden = true;
        }
        else
        {
            t.hidden = false;
        }
    }
}

void Mesh::moveBVH(BoundingBox* box, vec3 amount)
{
    box->min += amount;
    box->max += amount;
    
    if (box->sub[0] != nullptr)
    {
        moveBVH(box->sub[0], amount);
    }
    
    if (box->sub[1] != nullptr)
    {
        moveBVH(box->sub[1], amount);
    }
}

void Mesh::setPosition(vec3 position)
{
    moveBVH(m_mainbox, position - m_position);
    m_position = position;
}

void Mesh::move(vec3 amount)
{
    moveBVH(m_mainbox, amount);
    m_position += amount;
}

void Mesh::rotate(vec3 axis, float radians)
{
    // TODO: apply the rotation at render time? storing and exposing euler angles
    
    quat q = glm::angleAxis(radians, glm::normalize(axis));
    
    for (Triangle& t : m_triangles)
    {
        for (Vertex& v : t.vertex)
        {
            v.position = q * v.position;
            v.normal = q * v.normal;
        }
        
        t.geometricNormal = q * t.geometricNormal;
    }
    
    // recompute the BVH (it is axis-aligned so cannot simply be rotated along with the mesh)
    delete m_mainbox;
    m_mainbox = new BoundingBox();
    computeBVH();
    moveBVH(m_mainbox, m_position);
}

/**
 * Scale the mesh by given factors
 */
void Mesh::scale(vec3 factors)
{
    for (Triangle& t : m_triangles)
    {
        for (Vertex& v : t.vertex)
        {
            v.position *= factors;
        }
    }
    
    // TODO: scale the BVH instead (like moveBVH)
    delete m_mainbox;
    m_mainbox = new BoundingBox();
    computeBVH();
}

void Mesh::scale(float factor)
{
    scale(vec3(factor));
}

void Mesh::flip(vec3 comps)
{
    // assuming comps has values xyz in {-1, 1}
    
    for (Triangle& t : m_triangles)
    {
        for (Vertex& v : t.vertex)
        {
            v.position *= comps;
            v.normal *= comps;
        }
        
        t.geometricNormal *= comps;
    }
    
    // TODO: flip the BVH instead (like moveBVH)
    delete m_mainbox;
    m_mainbox = new BoundingBox();
    computeBVH();
}

void Mesh::flipX()
{
    flip(vec3(-1, 1, 1));
}

void Mesh::flipY()
{
    flip(vec3(1, -1, 1));
}

void Mesh::flipZ()
{
    flip(vec3(1, 1, -1));
}

/**
 * Push a temporary vertex for triangle building given a position and a normal vector
 */
void Mesh::pushVertex(vec3 position, vec3 normal)
{
    m_temporaryVertices[m_numSetTemporaryVertices++ % 3] = Vertex(position, normal);
}

/**
 * Push a temporary vertex for triangle building given a position, a normal vector and texture coordinates
 */
void Mesh::pushVertex(vec3 position, vec3 normal, vec2 textureCoords)
{
    m_temporaryVertices[m_numSetTemporaryVertices++ % 3] = Vertex(position, normal, textureCoords);
}

/**
 * Commit the currently pushed temporary vertices as a triangle using the given material
 * If fewer than three temporary vertices have been pushed when this function is called, no
 * triangle will be created. If more than three vertices have been pushed, the last three
 * will be used to create the triangle.
 */
void Mesh::commitTriangle(Material mat)
{
    m_triangles.push_back(
        Triangle(m_temporaryVertices[0], m_temporaryVertices[1], m_temporaryVertices[2], mat));
    
    m_numSetTemporaryVertices = 0;
}

/**
 * Flip the Y-coordinate of all positions and normal vectors
 */
// void Mesh::flipY()
// {

// }

/**
 * Invert all the normals, e.g vn = -vn
 */
void Mesh::flipNormals()
{
    for (Triangle& t : m_triangles)
    {
        for (Vertex& v : t.vertex)
        {
            v.normal = -v.normal;
        }
    }
}

/**
 * Colorize the mesh by modifying the diffuse, specular and ambient components of each triangle material
 */
void Mesh::colorize()
{
    vector<vec3> colors;
    colors.push_back(vec3(0,0,1));
    colors.push_back(vec3(0,0,1));
    colors.push_back(vec3(0,1,0));
    colors.push_back(vec3(0,1,0));
    colors.push_back(vec3(0,1,1));
    colors.push_back(vec3(0,1,1));
    colors.push_back(vec3(1,0,0));
    colors.push_back(vec3(1,0,0));
    colors.push_back(vec3(1,0,1));
    colors.push_back(vec3(1,0,1));
    colors.push_back(vec3(1,1,0));
    colors.push_back(vec3(1,1,0));
    
    int color = 0;
    
    for (Triangle& t : m_triangles)
    {
        t.material.ambientFactors = colors[color % colors.size()] * 0.25f;
        t.material.diffuseFactors = colors[color % colors.size()] * 0.25f;
        t.material.specularFactors = vec3(1.0f);
        ++color;
    }
}

/**
 * Apply a given material to all triangles of the mesh
 */
// TODO: make this part of the Object interface
void Mesh::applyMaterial(Material mat)
{
    for (Triangle& t : m_triangles)
    {
        t.material = mat;
    }
}

/**
 * Compute the bounding volume hierarchy used to speed up intersection tests
 */
void Mesh::computeBVH()
{
    // compute the main bounding box
    for (Triangle& t : m_triangles)
    {
        m_mainbox->triangles.push_back(&t);
        
        for (int i = 0; i < 3; ++i)
        {
            m_mainbox->min.x = glm::min(m_mainbox->min.x, t.vertex[i].position.x);
            m_mainbox->min.y = glm::min(m_mainbox->min.y, t.vertex[i].position.y);
            m_mainbox->min.z = glm::min(m_mainbox->min.z, t.vertex[i].position.z);
            
            m_mainbox->max.x = glm::max(m_mainbox->max.x, t.vertex[i].position.x);
            m_mainbox->max.y = glm::max(m_mainbox->max.y, t.vertex[i].position.y);
            m_mainbox->max.z = glm::max(m_mainbox->max.z, t.vertex[i].position.z);
        }
    }
    
    // inflate the main bounding box slightly if it has zero size in some dimension
    if (m_mainbox->min.x == m_mainbox->max.x)
    {
        m_mainbox->min.x -= 0.1f;
        m_mainbox->max.x += 0.1f;
    }
    
    if (m_mainbox->min.y == m_mainbox->max.y)
    {
        m_mainbox->min.y -= 0.1f;
        m_mainbox->max.y += 0.1f;
    }
    
    if (m_mainbox->min.z == m_mainbox->max.z)
    {
        m_mainbox->min.z -= 0.1f;
        m_mainbox->max.z += 0.1f;
    }
    
    // compute BVH
    BVH(m_mainbox, BVHCut::X, 0);
}

/**
 * Triangle-box intersection test. Returns True if the triangle t intersects the bounding box b, False otherwise.
 * Source: http://fileadmin.cs.lth.se/cs/personal/tomas_akenine-moller/code/tribox_tam.pdf
 * Source: http://fileadmin.cs.lth.se/cs/personal/tomas_akenine-moller/code/tribox3.txt
 */
bool Mesh::triangleInBox(Triangle* t, BoundingBox* box)
{
    float boxhalfsize[3] = {(box->max.x - box->min.x)/2.0f, (box->max.y - box->min.y)/2.0f, (box->max.z - box->min.z)/2.0f};
    
    float boxcenter[3] = {box->min.x + boxhalfsize[0], box->min.y + boxhalfsize[1], box->min.z + boxhalfsize[2]};
    
    float triverts[3][3] = {
        {t->vertex[0].position.x, t->vertex[0].position.y, t->vertex[0].position.z},
        {t->vertex[1].position.x, t->vertex[1].position.y, t->vertex[1].position.z},
        {t->vertex[2].position.x, t->vertex[2].position.y, t->vertex[2].position.z}};
    
    return triBoxOverlap(boxcenter, boxhalfsize, triverts) == 1;
}

/**
 * BVH subdivision helper, creates a new BoundingBox with corners at min and max, which are
 * supposed to be contained within parent. Checks all triangles of parent for intersections
 * with new bounding box. If the resulting box is empty, returns nullptr, otherwise returns
 * pointer to the new box.
 */
Mesh::BoundingBox* Mesh::BVHSubdivide(BoundingBox* parent, vec3 min, vec3 max)
{
    BoundingBox* result = new BoundingBox;
    
    result->min = min;
    result->max = max;
    
    // test all triangles contained in parent box to see which ones intersect this new box
    for (Triangle* t : parent->triangles)
    {
        if (triangleInBox(t, result))
        {
            result->triangles.push_back(t);
        }
    }
    
    // return new box if not empty, nullptr otherwise
    if (result->triangles.size() > 0)
    {
        return result;
    }
    else
    {
        delete result;
        return nullptr;
    }
}

/**
 * Main recursive BVH algorithm entry point. Subdivides the given box according to the
 * specified cut, calls sub on each subdivision and recursively calls itself on any
 * subboxes that ended up containing triangles. Aborts recursion at a fixed depth or
 * if given box contains a small enough number of triangles.
 */
void Mesh::BVH(BoundingBox* box, BVHCut cut, uint depth)
{
    // compute half lengths of the given box in each dimension
    vec3 halfsize(abs(box->max.x - box->min.x)/2.0f, abs(box->max.y - box->min.y)/2.0f,
                  abs(box->max.z - box->min.z)/2.0f);
    
    // abort recursion
    if (depth > 16 || (depth > 0 && box->triangles.size() < 25))
    {
        return;
    }
    
    BVHCut nextcut;
    
    switch (cut)
    {
        case X: // cut along plane x=boxcenter.x
            box->sub[0] = BVHSubdivide(box, box->min, vec3(box->min.x + halfsize.x, box->max.y, box->max.z));
            box->sub[1] = BVHSubdivide(box, vec3(box->min.x + halfsize.x, box->min.y, box->min.z), box->max);
            nextcut = Y;
            break;
        
        case Y: // cut along plane y=boxcenter.y
            box->sub[0] = BVHSubdivide(box, box->min, vec3(box->max.x, box->min.y + halfsize.y, box->max.z));
            box->sub[1] = BVHSubdivide(box, vec3(box->min.x, box->min.y + halfsize.y, box->min.z), box->max);
            nextcut = Z;
            break;
        
        case Z: // cut along plane z=boxcenter.z
            box->sub[0] = BVHSubdivide(box, box->min, vec3(box->max.x, box->max.y, box->min.z + halfsize.z));
            box->sub[1] = BVHSubdivide(box, vec3(box->min.x, box->min.y, box->min.z + halfsize.z), box->max);
            nextcut = X;
            break;
        
        default:
            nextcut = X;
            break;
    }
    
    // recursively compute BVH on any non-empty subboxes
    if (box->sub[0] != nullptr)
    {
        box->hasSubs = true;
        BVH(box->sub[0], nextcut, depth + 1);
    }
    
    if (box->sub[1] != nullptr)
    {
        box->hasSubs = true;
        BVH(box->sub[1], nextcut, depth + 1);
    }
}

/**
 * Ray-box intersection test
 * Source: https://tavianator.com/fast-branchless-raybounding-box-intersections/
 */
bool Mesh::rayIntersectsBox(vec3 from, vec3 dir_inv, BoundingBox* box)
{
    float t1 = (box->min[0] - from[0])*dir_inv[0];
    float t2 = (box->max[0] - from[0])*dir_inv[0];
 
    float tmin = glm::min(t1, t2);
    float tmax = glm::max(t1, t2);
 
    for (int i = 1; i < 3; ++i)
    {
        t1 = (box->min[i] - from[i])*dir_inv[i];
        t2 = (box->max[i] - from[i])*dir_inv[i];
        
        tmin = glm::max(tmin, glm::min(t1, t2));
        tmax = glm::min(tmax, glm::max(t1, t2));
    }
 
    return tmax > glm::max(tmin, 0.0f);
}

/**
 * Wireframe generator for the BVH tree, used for debugging
 */
/*void Mesh::bboxwire(vector<WireframeElement>& stuff, BoundingBox* box)
{
    WireframeElement e;
    
    e.points.push_back(vec3(box->min.x, box->min.y, box->min.z));
    e.points.push_back(vec3(box->max.x, box->min.y, box->min.z));
    e.points.push_back(vec3(box->max.x, box->max.y, box->min.z));
    e.points.push_back(vec3(box->min.x, box->max.y, box->min.z));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    
    stuff.push_back(e);
    
    e.points.clear();
    e.normals.clear();
    
    e.points.push_back(vec3(box->min.x, box->min.y, box->min.z));
    e.points.push_back(vec3(box->min.x, box->min.y, box->max.z));
    e.points.push_back(vec3(box->min.x, box->max.y, box->max.z));
    e.points.push_back(vec3(box->min.x, box->max.y, box->min.z));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    
    stuff.push_back(e);
    
    e.points.clear();
    e.normals.clear();
    
    e.points.push_back(vec3(box->min.x, box->min.y, box->max.z));
    e.points.push_back(vec3(box->max.x, box->min.y, box->max.z));
    e.points.push_back(vec3(box->max.x, box->max.y, box->max.z));
    e.points.push_back(vec3(box->min.x, box->max.y, box->max.z));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    
    stuff.push_back(e);
    
    e.points.clear();
    e.normals.clear();
    
    e.points.push_back(vec3(box->max.x, box->min.y, box->min.z));
    e.points.push_back(vec3(box->max.x, box->min.y, box->max.z));
    e.points.push_back(vec3(box->max.x, box->max.y, box->max.z));
    e.points.push_back(vec3(box->max.x, box->max.y, box->min.z));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    e.normals.push_back(vec3(0,0,-1));
    
    stuff.push_back(e);
    
    e.points.clear();
    e.normals.clear();
    
    for (int i = 0; i < 2; ++i)
    {
        if (box->sub[i] != nullptr)
        {
            bboxwire(stuff, box->sub[i]);
        }
    }
}*/

// debugging purposes
/*void Mesh::printTriangle(Triangle t)
{
    printf("Triangle v0:(%.2f, %.2f, %.2f) normal(%.2f, %.2f, %.2f)\n         v1:(%.2f, %.2f, %.2f) normal(%.2f, %.2f, %.2f)\n         v2:(%.2f, %.2f, %.2f) normal(%.2f, %.2f, %.2f)\n",
           t.vertex[0].position.x, t.vertex[0].position.y, t.vertex[0].position.z,
           t.vertex[0].normal.x, t.vertex[0].normal.y, t.vertex[0].normal.z,
           t.vertex[1].position.x, t.vertex[1].position.y, t.vertex[1].position.z,
           t.vertex[1].normal.x, t.vertex[1].normal.y, t.vertex[1].normal.z,
           t.vertex[2].position.x, t.vertex[2].position.y, t.vertex[2].position.z,
           t.vertex[2].normal.x, t.vertex[2].normal.y, t.vertex[2].normal.z);
}*/
