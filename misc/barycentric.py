#!/usr/bin/python3
# -*- coding: utf-8 -*-

# *neode.command* setgo python3 barycentric.py

from vec3 import vec3

# https://gamedev.stackexchange.com/a/23745
# Compute barycentric coordinates (u, v, w) for
# point p with respect to triangle (a, b, c)
# def Barycentric(Point p, Point a, Point b, Point c, float &u, float &v, float &w)
def Barycentric(p, a, b, c):
    # Vector v0 = b - a
    v0 = b - a
    
    # Vector v1 = c - a
    v1 = c - a
    
    # Vector v2 = p - a
    v2 = p - a
    
    d00 = v0.dot(v0)
    d01 = v0.dot(v1)
    d11 = v1.dot(v1)
    d20 = v2.dot(v0)
    d21 = v2.dot(v1)
    
    denom = d00 * d11 - d01 * d01
    v = (d11 * d20 - d01 * d21) / denom
    w = (d00 * d21 - d01 * d20) / denom
    u = 1.0 - v - w
    
    return vec3(u,v,w)







a = vec3(-0.5, -0.5, 0.0)
b = vec3(0.5, -0.5, 0.0)
c = vec3(-0.5, 0.5, 0.0)

p = vec3(0.0, 0.0, 0.0)

sol = Barycentric(p,a,b,c)
print(sol)
# print(a*sol.x + b*sol.y + c*sol.z)
