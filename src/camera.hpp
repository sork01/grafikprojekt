
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only camera.hpp

#ifndef HAVE_CAMERA_H
#define HAVE_CAMERA_H

#include "glm.hpp"

using glm::vec3;
using glm::mat3;

/**
 * Camera class
 * 
 * This should be self-documenting enough to not need comments
 */
class Camera
{
public:
    Camera();
    virtual ~Camera(){}
    
    void setFocalLength(float length);
    float getFocalLength() const;
    
    void setPosition(vec3 position);
    void setPosition(float x, float y, float z);
    vec3 getPosition() const;
    void moveLeft(float units);
    void moveRight(float units);
    void moveForward(float units);
    void moveBack(float units);
    void moveUp(float units);
    void moveDown(float units);
    
    void setPitch(float pitch);
    float getPitch() const;
    void pitchUp(float radians);
    void pitchDown(float radians);
    
    void setYaw(float yaw);
    float getYaw() const;
    void turnLeft(float radians);
    void turnRight(float radians);
    
    //void setRoll(float roll);
    
    vec3 transformPoint(vec3 point) const;
    mat3 getRotationMatrix() const;

private:
    float focalLength;
    vec3 position;
    mat3 rotation;
    float pitch;
    float yaw;
    
    void updateRotationMatrix();
};

#endif
