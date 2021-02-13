
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only mesh.hpp

#ifndef HAVE_MESH_H
#define HAVE_MESH_H

#include "object.hpp"
#include <iostream>

using glm::vec3;

/**
 * Mesh represents an object defined by a set of triangles.
 */
class Mesh : public Object
{
public:
    /**
     * Create a new empty mesh
     */
    Mesh();
    
    ~Mesh();
    
    /**
     * Retrieve the wireframe elements of the mesh
     */
    void getWireframe(vector<WireframeElement>& stuff);
    
    /**
     * Find the closest intersection given a ray origin and a direction
     */
    bool getIntersection(vec3 from, vec3 dir, Intersection& is);
    
    /**
     * Remove invisible triangles from intersection testing
     */
    void cullInvisibleParts(vec3 cameraPosition);
    
    /**
     * Push a temporary vertex for triangle building given a position and a normal vector
     */
    void pushVertex(vec3 position, vec3 normal);
    
    /**
     * Push a temporary vertex for triangle building given a position, a normal vector and texture coordinates
     */
    void pushVertex(vec3 position, vec3 normal, vec2 textureCoords);
    
    /**
     * Commit the currently pushed temporary vertices as a triangle using the given material
     * If fewer than three temporary vertices have been pushed when this function is called, no
     * triangle will be created. If more than three vertices have been pushed, the last three
     * will be used to create the triangle.
     */
    void commitTriangle(Material mat);
    
    /**
     * Flip the Y-coordinate of all positions and normal vectors
     */
    void flipY();
    
    /**
     * Invert all the normals, e.g vn = -vn
     */
    void flipNormals();
    
    /**
     * Scale the mesh by a given factor
     */
    void scale(float factor);
    
    /**
     * Colorize the mesh by modifying the diffuse, specular and ambient components of each triangle material
     */
    void colorize();
    
    /**
     * Apply a given material to all triangles of the mesh
     */
    void applyMaterial(Material mat);
    
    /**
     * Compute the bounding volume hierarchy used to speed up intersection tests
     */
    void computeBVH();
    
private:
    /**
     * The internal representation of a single vertex
     */
    struct Vertex
    {
        Vertex(){}
        Vertex(vec3 position, vec3 normal) : Vertex(position, normal, vec2(0,0)) {}
        Vertex(vec3 position, vec3 normal, vec2 textureCoords) : position(position), normal(normal), textureCoords(textureCoords) {}
        
        vec3 position;
        vec3 normal;
        vec2 textureCoords;
    };
    
    /**
     * The internal representation of a single triangle
     */
    struct Triangle
    {
        Triangle() : Triangle(Vertex(vec3(0,-1,0), vec3(0,0,-1)),
                              Vertex(vec3(-1,0,0), vec3(0,0,-1)),
                              Vertex(vec3(1,0,0), vec3(0,0,-1))) {}
        
        Triangle(Vertex v0, Vertex v1, Vertex v2) : Triangle(v0, v1, v2, Material(vec3(1,1,1))) {}
        
        Triangle(Vertex v0, Vertex v1, Vertex v2, Material mat) : material(mat)
        {
            vertex[0] = v0;
            vertex[1] = v1;
            vertex[2] = v2;
            
            // compute the surface normal
            vec3 e1 = v1.position - v0.position;
            vec3 e2 = v2.position - v0.position;
            
            geometricNormal = glm::normalize(glm::cross(e1, e2));
        }
        
        Vertex vertex[3];
        Material material;
        vec3 geometricNormal;
        
        // for culling
        bool hidden = false;
    };
    
    /**
     * The internal representation of a bounding box used for BVH
     */
    struct BoundingBox
    {
        /**
         * The left-top-front corner of the box
         */
        vec3 min;
        
        /**
         * The right-bottom-back corner of the box
         */
        vec3 max;
        
        /**
         * The triangles contained in this box
         */
        vector<Triangle*> triangles;
        
        /**
         * Indicating whether any subboxes are != nullptr
         */
        bool hasSubs = false;
        
        /**
         * Two boxes that subdivide this box
         */
        BoundingBox* sub[2] = {nullptr, nullptr};
    };
    
    /**
     * The direction of a cut used when computing subdivisions in the BVH algorithm.
     * For each V in {X,Y,Z}, a BVHCut of V indicates a cut along the plane V=boxcenter.V
     */
    enum BVHCut {X,Y,Z};
    
    /**
     * Compute the barycentric coordinates of a point p given triangle vertices a, b and c
     */
    vec3 computeBarycentricCoords(vec3 p, vec3 a, vec3 b, vec3 c);
    
    /**
     * Triangle-box intersection test. Returns True if the triangle t intersects the bounding box b, False otherwise.
     */
    bool triangleInBox(Triangle* t, BoundingBox* b);
    
    /**
     * BVH subdivision helper, creates a new BoundingBox with corners at min and max, which are
     * supposed to be contained within parent. Checks all triangles of parent for intersections
     * with new bounding box. If the resulting box is empty, returns nullptr, otherwise returns
     * pointer to the new box.
     */
    BoundingBox* BVHSubdivide(BoundingBox* parent, vec3 min, vec3 max);
    
    /**
     * Main recursive BVH algorithm entry point. Subdivides the given box according to the
     * specified cut, calls sub on each subdivision and recursively calls itself on any
     * subboxes that ended up containing triangles. Aborts recursion at a fixed depth or
     * if given box contains a small enough number of triangles.
     */
    void BVH(BoundingBox* box, BVHCut cut, uint depth);
    
    /**
     * Ray-box intersection test
     */
    bool rayIntersectsBox(vec3 from, vec3 dir_inv, BoundingBox* box);
    
    /**
     * Real intersection tester that applies the BVH optimization
     */
    bool getIntersection(vec3 start, vec3 dir, vec3 dir_inv, Intersection& is, BoundingBox* box);
    
    /**
     * All the mesh's triangles.
     */
    vector<Triangle> triangles;
    
    /**
     * For keeping track of pushVertex/commitTriangle
     */
    int numSetTemporaryVertices = 0;
    Vertex temporaryVertices[3];
    
    /**
     * Main bounding box which contains the entire mesh
     */
    BoundingBox mainbox;
    
    /**
     * Wireframe generator for the BVH tree, used for debugging
     */
    /*void bboxwire(vector<WireframeElement>&, BoundingBox*);*/
    
    // debugging purposes
    /*void printTriangle(Triangle t);*/
};

#endif
