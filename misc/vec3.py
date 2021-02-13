#!/usr/bin/python3
# -*- coding: utf-8 -*-

# *neode.onsave* setgo python3 vec3.py

import math

class vec3:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z
    
    def __add__(self, other):
        if type(other) is vec3:
            return vec3(self.x + other.x, self.y + other.y, self.z + other.z)
        else:
            return vec3(self.x + other, self.y + other, self.z + other)
    
    def __sub__(self, other):
        if type(other) is vec3:
            return vec3(self.x - other.x, self.y - other.y, self.z - other.z)
        else:
            return vec3(self.x - other, self.y - other, self.z - other)
    
    def __floordiv__(self, other):
        if type(other) is float or type(other) is int:
            return vec3(math.floor(self.x/other), math.floor(self.y/other), math.floor(self.z/other))
        else:
            raise ArgumentError()
    
    def __truediv__(self, other):
        if type(other) is float:
            return vec3(self.x/other, self.y/other, self.z/other)
        else:
            raise ArgumentError()
    
    def __mul__(self, other):
        if (type(other) is vec3):
            return vec3(self.x * other.x, self.y * other.y, self.z * other.z)
        else:
            return vec3(self.x * other, self.y * other, self.z * other)
    
    def __repr__(self):
        return "vec3({}, {}, {})".format(self.x, self.y, self.z)    
    
    def __str__(self):
        return "vec3({:.2f}, {:.2f}, {:.2f})".format(self.x, self.y, self.z)
    
    def cross(self, other):
        return vec3(self.y*other.z - self.z*other.y, self.z*other.x - self.x*other.z, self.x*other.y - self.y*other.x)
    
    def dot(self, other):
        return self.x * other.x + self.y * other.y + self.z * other.z
    
    def length(self):
        return math.sqrt(self.x*self.x + self.y*self.y + self.z*self.z)
    
    def normalize(self):
        return self / self.length()
