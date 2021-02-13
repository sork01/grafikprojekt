
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only sphere.cpp

#include "sphere.hpp"

using glm::vec3;

/**
 * Create a new sphere at the given position, of the given radius and using the given material
 */
Sphere::Sphere(vec3 position, float radius, Material material)
    : m_position(position), m_radius(radius), m_material(material)
{
    m_rotation = glm::angleAxis(0.0f, glm::normalize(vec3(1,0,0)));
}

Sphere::~Sphere()
{
    
}

/**
 * Get a wireframe representation of the sphere
 */
void Sphere::getWireframe(vector<WireframeElement>& stuff)
{
    // spherical coordinates: 
    // x = R sin p cos t
    // y = R sin p sin t
    // z = R cos p
    
    // we want this amount of vertical circles along with the same amount of horizontal circles
    int num_circles = 10;
    int num_points_per_circle = 20;
    
    float step_phi = 2.0f*M_PI / float((num_circles * 2) - 1);
    float step_theta = 2.0f*M_PI / float(num_points_per_circle);
    
    float phi = 0.0f;
    float theta = 0.0f;
    
    // one list of points for each "great circle", meaning the vertical full circles
    WireframeElement greats[num_points_per_circle];
    
    for (int p = 0; p < num_circles * 2; ++p)
    {
        // outer loops steps us thru a set of angles between us and the z axis
        
        WireframeElement e;
        
        theta = 0.0f;
        
        for (int t = 0; t < num_points_per_circle; ++t)
        {
            // inner loop rotates us around the z axis
            
            vec3 v = vec3(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta));
            
            e.points.push_back(m_position + m_radius * v);
            e.normals.push_back(v);
            
            greats[t].points.push_back(m_position + m_radius * v);
            greats[t].normals.push_back(v);
            
            theta += step_theta;
        }
        
        stuff.push_back(e);
        
        phi += step_phi;
    }
    
    // dont forget to add the great circles now that they're all done
    for (int i = 0; i < num_points_per_circle; ++i)
    {
        stuff.push_back(greats[i]);
    }
}

void Sphere::cullInvisibleParts(vec3 cameraPosition)
{
}

#define SQ(x) x*x

/**
 * Ray-sphere intersection test
 */
bool Sphere::getIntersection(vec3 from, vec3 dir, Intersection& is, bool enableCulling)
{
    // TODO: should this be enabled? maybe configurable per-object?
    // if (glm::distance(from, m_position) < m_radius)
    // {
        // return false;
    // }
    
    float a = SQ(dir.x) + SQ(dir.y) + SQ(dir.z);
    
    float b = 2.0f * (from.x*dir.x + from.y*dir.y + from.z*dir.z
                      - dir.x*m_position.x - dir.y*m_position.y - dir.z*m_position.z);
    
    float c = SQ(from.x) + SQ(from.y) + SQ(from.z)
            + SQ(m_position.x) + SQ(m_position.y) + SQ(m_position.z)
            - 2.0f * (from.x*m_position.x + from.y*m_position.y + from.z*m_position.z)
            - SQ(m_radius);
    
    float b2 = SQ(b);
    float ac4 = 4.0f*a*c;
    
    if (ac4 > b2)
    {
        // no real roots
        return false;
    }
    else
    {
        float x1 = (-b + sqrt(b2 - ac4)) / (2.0f * a);
        float x2 = (-b - sqrt(b2 - ac4)) / (2.0f * a);
        
        // we only consider rays going forwards
        if (x1 < 0 && x2 < 0)
        {
            return false;
        }
        
        // find the closest point
        is.distance = std::numeric_limits<float>::max();
        is.material = m_material;
        
        if (x1 >= 0)
        {
            is.position = from + x1*dir;
            is.distance = glm::distance(from, is.position);
        }
        
        if (x2 >= 0)
        {
            vec3 p2 = from + x2*dir;
            float r2 = glm::distance(from, p2);
            
            if (r2 < is.distance)
            {
                is.position = p2;
                is.distance = r2;
            }
        }
        
        is.normal = glm::normalize(is.position - m_position);
        
        // https://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere
        
        vec3 texangle = m_rotation * is.normal;
        
        // is.textureCoords = vec2(0.5f + atan2(is.normal.z, is.normal.x) / (2.0f*M_PI), 0.5f + asin(is.normal.y) / M_PI);
        is.textureCoords = vec2(0.5f + atan2(texangle.z, texangle.x) / (2.0f*M_PI), 0.5f + asin(texangle.y) / M_PI);
        
        return true;
    }
}

void Sphere::setRadius(float radius)
{
    m_radius = radius;
}

void Sphere::setPosition(vec3 pos)
{
    m_position = pos;
}

void Sphere::move(vec3 amount)
{
    m_position += amount;
}

void Sphere::rotate(vec3 axis, float radians)
{
    quat q = glm::angleAxis(radians, glm::normalize(axis));
    m_rotation = m_rotation * q;
}

void Sphere::scale(vec3 factors)
{
    scale((factors.x + factors.y + factors.z) / 3.0f);
}

void Sphere::scale(float factor)
{
    m_radius *= factor;
}

void Sphere::flipX()
{
    
}

void Sphere::flipY()
{
    
}

void Sphere::flipZ()
{
    
}

