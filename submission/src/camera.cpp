
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only camera.cpp

#include "camera.hpp"
#include <iostream>

using namespace std;

// This should be self-documenting enough to not need comments

Camera::Camera()
{
    focalLength = 500;
    position = vec3(0, 0, 0);
    rotation = mat3(1.0f);
    pitch = 0.0f;
    yaw = 0.0f;
}

void Camera::updateRotationMatrix()
{
    mat3 rx = mat3(vec3(1.0f, 0.0f, 0.0f),
              vec3(0.0f, cos(pitch), -sin(pitch)),
              vec3(0.0f, sin(pitch), cos(pitch)));
    
    mat3 ry = mat3(vec3(cos(yaw), 0.0f, sin(yaw)),
              vec3(0.0f, 1.0f, 0.0f),
              vec3(-sin(yaw), 0.0f, cos(yaw)));
    
    rotation = ry * rx;
}

void Camera::setFocalLength(float length)
{
    focalLength = length; // TODO: sanity check?
}

float Camera::getFocalLength()
{
    return focalLength;
}

void Camera::setPosition(vec3 pos)
{
    position = pos;
}

void Camera::setPosition(float x, float y, float z)
{
    position = vec3(x, y, z);
}

vec3 Camera::getPosition()
{
    return position;
}

void Camera::moveLeft(float units)
{
    position -= units * rotation[0];
}

void Camera::moveRight(float units)
{
    position += units * rotation[0];
}

void Camera::moveForward(float units)
{
    position += units * rotation[2];
}

void Camera::moveBack(float units)
{
    position -= units * rotation[2];
}

void Camera::moveUp(float units)
{
    position -= units * rotation[1];
}

void Camera::moveDown(float units)
{
    position += units * rotation[1];
}

void Camera::setPitch(float pitch)
{
    this->pitch = pitch;
    updateRotationMatrix();
}

float Camera::getPitch()
{
    return pitch;
}

void Camera::pitchUp(float radians)
{
    setPitch(pitch - radians);
}

void Camera::pitchDown(float radians)
{
    setPitch(pitch + radians);
}

void Camera::setYaw(float yaw)
{
    this->yaw = yaw;
    updateRotationMatrix();
}

float Camera::getYaw()
{
    return yaw;
}

void Camera::turnLeft(float radians)
{
    setYaw(yaw + radians);
}

void Camera::turnRight(float radians)
{
    setYaw(yaw - radians);
}

vec3 Camera::transformPoint(vec3 point)
{
    return (point - position) * rotation;
}

mat3 Camera::getRotationMatrix()
{
    return rotation;
}
