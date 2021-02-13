scene_clear()
scene_set_ambient_light(0.25, 0.25, 0.25)

option("ray", "ShadowSamples", 16)

camera_set_pos(0, -6, -6.3)
camera_set_yaw(0.0)
camera_set_pitch(0.9)

light = add_light()
light_set_pos(light, 0, -2.5, -5)
light_set_diffuse(light, 10,10,10)
light_set_specular(light, 10,10,10)

floormat = add_material()
material_set_ambient(floormat, 0.45, 0.45, 0.45)
material_set_diffuse(floormat, 1,1,1)
material_set_specular(floormat, 1,1,1)
material_set_shininess(floormat, 100)

floor = add_mesh()
scale = 6

vertex_pos(scale,1,scale); vertex_normal(0,-1,0); mesh_add_vertex(floor)
vertex_pos(-scale,1,scale); vertex_normal(0,-1,0); mesh_add_vertex(floor)
vertex_pos(-scale,1,-scale); vertex_normal(0,-1,0); mesh_add_vertex(floor)
mesh_commit_triangle(floor, floormat)

vertex_pos(scale,1,scale); vertex_normal(0,-1,0); mesh_add_vertex(floor)
vertex_pos(-scale,1,-scale); vertex_normal(0,-1,0); mesh_add_vertex(floor)
vertex_pos(scale,1,-scale); vertex_normal(0,-1,0); mesh_add_vertex(floor)
mesh_commit_triangle(floor, floormat)

mesh_finalize(floor)

spheremat = add_material()
material_set_ambient(spheremat, 0.45, 0.45, 0.45)
material_set_diffuse(spheremat, 2,2,2)
material_set_specular(spheremat, 5,5,5)
material_set_shininess(spheremat, 100)

spheremiddle = add_sphere(0,0.25,0, 0.75, spheremat)
sphereleft = add_sphere(-2.5,0.5,0, 0.5, spheremat)
sphereright = add_sphere(2.5,0.5,0, 0.5, spheremat)
