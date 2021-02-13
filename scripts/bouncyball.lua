scene_clear()
scene_set_ambient_light(0.25, 0.25, 0.25)

option("ray", "ShadowSamples", 16)

camera_set_pos(0, 0, -20.001)
camera_set_yaw(0.0)
camera_set_pitch(0.0)

light = add_light()
light_set_pos(light, 0, -5, -6)
light_set_diffuse(light, 35,35,44)
light_set_specular(light, 35,35,35)

floormat = add_material()
material_set_ambient(floormat, 0.25, 0.25, 0.25)
material_set_diffuse(floormat, 1,1,1)
material_set_specular(floormat, 1,1,1)
material_set_shininess(floormat, 1)
material_set_reflectivity(floormat, 0.9, 0.9, 0.9)

floormesh = add_mesh()

scale = 20
y = 3.3

a = {scale, y, scale}
b = {-scale, y, scale}
c = {-scale, y, -scale}
d = {scale, y, -scale}

vertex_pos(a[1],a[2],a[3]); vertex_normal(0,-1,0); vertex_texture_coords(4,0); mesh_add_vertex(floormesh)
vertex_pos(b[1],b[2],b[3]); vertex_normal(0,-1,0); vertex_texture_coords(0,0); mesh_add_vertex(floormesh)
vertex_pos(c[1],c[2],c[3]); vertex_normal(0,-1,0); vertex_texture_coords(0,4); mesh_add_vertex(floormesh)
mesh_commit_triangle(floormesh, floormat)

vertex_pos(d[1],d[2],d[3]); vertex_normal(0,-1,0); vertex_texture_coords(4,4); mesh_add_vertex(floormesh)
vertex_pos(a[1],a[2],a[3]); vertex_normal(0,-1,0); vertex_texture_coords(4,0); mesh_add_vertex(floormesh)
vertex_pos(c[1],c[2],c[3]); vertex_normal(0,-1,0); vertex_texture_coords(0,4); mesh_add_vertex(floormesh)
mesh_commit_triangle(floormesh, floormat)

mesh_finalize(floormesh);

spheremat = add_material()
material_set_ambient(spheremat, 0.25, 0.15, 0.15)
material_set_diffuse(spheremat, 1.85, 0.45, 0.45)
material_set_specular(spheremat, 4,4,4)
material_set_shininess(spheremat, 100)

sphere_radius = 1
sphere_pos = {-16, y - 6*sphere_radius, 10}
sphere_vel = {0.2 / 2.0, -0.4 / 2.0, -0.15 / 2.0}

sp1 = add_sphere(sphere_pos[1], sphere_pos[2], sphere_pos[3], sphere_radius, spheremat)

for i=1,245 do
    sphere_pos[1] = sphere_pos[1] + sphere_vel[1]
    sphere_pos[2] = sphere_pos[2] + sphere_vel[2]
    sphere_pos[3] = sphere_pos[3] + sphere_vel[3]
    
    if sphere_pos[2] > (y - sphere_radius) then
        over = sphere_pos[2] - (y - sphere_radius)
        timeover = over / sphere_vel[2]
        
        sphere_pos[2] = sphere_pos[2] - sphere_vel[2] * timeover
        
        sphere_vel[2] = -(sphere_vel[2] * 0.84)
        
        sphere_pos[2] = sphere_pos[2] + sphere_vel[2] * timeover
    end
    
    sphere_set_pos(sp1, sphere_pos[1], sphere_pos[2], sphere_pos[3])
    sphere_vel[2] = sphere_vel[2] + (0.05 / 2.0)
    
    
    render()
    savebmp("image" .. i .. ".bmp")
end

