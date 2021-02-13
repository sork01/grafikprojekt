Escape: Quit

WASD + (2,X):   Camera movement, 2 moves up and X moves down
Arrow keys:     Camera pitch and yaw

Y,I:    Move movable light forward,back
U,N:    Move movable light up,down
H,J:    Move movable light left,right

R:  Switch between continuous wireframe rendering and continuous raytracing rendering
T:  Raytrace a single frame, press T again to return to wireframe mode

F:  Open file dialog, valid files are .STL, .OBJ and .LUA
    Note that some models loaded from .STL and .OBJ might have problems with vertex orders
    and/or normals. There are functions for dealing with these problems in the code but
    there is currently no way of accessing them in the UI.

Q:  Remove all loaded objects

F5: (re)load and run .LUA script. By default will try to load and run "script.lua"
    If you have selected a .LUA file in the file dialog this file will be used instead
