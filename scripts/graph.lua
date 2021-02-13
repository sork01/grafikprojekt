------------------------------------------

function f(x,y)
    -- change me!
    return math.cos(x)*math.sin(y)*2.0*(1.0 - ((x*x+y*y)/200.0))
end

-- reset view on reload?
reset_camera = true

-- change us!
xmin = -13
xmax = 13
zmin = -10
zmax = 10

-- resolution!
delta = 0.5

------------------------------------------

print("graph.lua: number of triangles: " .. math.floor((xmax-xmin)*(1.0/delta)*(zmax-zmin)*(1.0/delta)*2.0))

scene_clear()
scene_set_ambient_light(1,1,1)

light = add_light()
light_set_pos(light, 0, -10, 0)
light_set_diffuse(light, 100,100,100)
light_set_specular(light, 100,100,100)

if reset_camera then
    camera_set_pos(-0.10, -13.18, -21.21)
    camera_set_pitch(0.6)
    camera_set_yaw(0)
end

values = {}
normals = {}

h = 0.001

for x=xmin,xmax,delta do
    values[x] = {}
    normals[x] = {}
    
    for z=zmin,zmax,delta do
        values[x][z] = f(x,z)
        normals[x][z] = {-((f(x+h,z)-f(x,z))/h), -1, -((f(x,z+h)-f(x,z))/h)}
        -- normals[x][z] = {-f1(x,z), -1, -f2(x,z)}
    end
end

graphtexture_png = load_texture("misc/graphtexture.png")

mat = add_material()
material_set_ambient(mat, 0.2,0.2,0.2)
material_set_ambient_texture(mat, graphtexture_png)
material_set_diffuse(mat, 0.5,0.5,0.5)
material_set_diffuse_texture(mat, graphtexture_png)
material_set_specular(mat, 1,1,1)
material_set_shininess(mat, 150)

mymesh = add_mesh()

tris = 0

for x=xmin,xmax,delta do
    for z=zmin,zmax,delta do
        if values[x][z+delta] ~= nil and values[x+delta] ~= nil and values[x+delta][z] ~= nil then
            vertex_pos(x, -values[x][z], z)
            vertex_normal(normals[x][z][1], normals[x][z][2], normals[x][z][3])
            vertex_texture_coords((x-xmin)/(xmax-xmin), 1.0-(z-zmin)/(zmax-zmin))
            mesh_add_vertex(mymesh)
            
            vertex_pos(x + delta, -values[x+delta][z], z)
            vertex_normal(normals[x+delta][z][1], normals[x+delta][z][2], normals[x+delta][z][3])
            vertex_texture_coords((x+delta-xmin)/(xmax-xmin), 1.0-(z-zmin)/(zmax-zmin))
            mesh_add_vertex(mymesh)
            
            vertex_pos(x, -values[x][z+delta], z + delta)
            vertex_normal(normals[x][z+delta][1], normals[x][z+delta][2], normals[x][z+delta][3])
            vertex_texture_coords((x-xmin)/(xmax-xmin), 1.0-(z+delta-zmin)/(zmax-zmin))
            mesh_add_vertex(mymesh)
            
            mesh_commit_triangle(mymesh, mat)
        end
        
        if values[x-delta] ~= nil and values[x-delta][z+delta] ~= nil and values[x][z+delta] ~= nil then
            vertex_pos(x, -values[x][z], z)
            vertex_normal(normals[x][z][1], normals[x][z][2], normals[x][z][3])
            vertex_texture_coords((x-xmin)/(xmax-xmin), 1.0-(z-zmin)/(zmax-zmin))
            mesh_add_vertex(mymesh)
            
            vertex_pos(x, -values[x][z+delta], z + delta)
            vertex_normal(normals[x][z+delta][1], normals[x][z+delta][2], normals[x][z+delta][3])
            vertex_texture_coords((x-xmin)/(xmax-xmin), 1.0-(z+delta-zmin)/(zmax-zmin))
            mesh_add_vertex(mymesh)
            
            vertex_pos(x - delta, -values[x-delta][z+delta], z + delta)
            vertex_normal(normals[x-delta][z+delta][1], normals[x-delta][z+delta][2], normals[x-delta][z+delta][3])
            vertex_texture_coords((x-delta-xmin)/(xmax-xmin), 1.0-(z+delta-zmin)/(zmax-zmin))
            mesh_add_vertex(mymesh)
            
            mesh_commit_triangle(mymesh, mat)
        end
    end
end

mesh_finalize(mymesh)
render()
