phase = 0

function f(x,y)
    return math.sin(phase)*(math.sin(x) * math.sin(y))
end

xmin = -10
xmax = 10
zmin = -10
zmax = 10

delta = 0.5



for i=0,200 do
    phase = phase + 0.05
    
    scene_clear()
    scene_set_ambient_light(0.25, 0.25, 0.25)

    light = add_light()
    light_set_pos(light, 0, -10, 0)
    light_set_diffuse(light, 100,100,100)
    light_set_specular(light, 100,100,100)

    camera_set_pos(-0.10, -13.18, -21.21)
    camera_set_pitch(0.6)
    camera_set_yaw(0)

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

    mat = add_material()
    material_set_ambient(mat, 0.5,0.5,1)
    material_set_diffuse(mat, 0.25, 0.25, 0.55)
    material_set_specular(mat, 1,1,1)
    material_set_shininess(mat, 150)

    mymesh = add_mesh()

    for x=xmin,xmax,delta do
        for z=zmin,zmax,delta do
            if values[x][z+delta] ~= nil and values[x+delta] ~= nil and values[x+delta][z] ~= nil then
                vertex_pos(x, -values[x][z], z)
                vertex_normal(normals[x][z][1], normals[x][z][2], normals[x][z][3])
                mesh_add_vertex(mymesh)
                
                vertex_pos(x + delta, -values[x+delta][z], z)
                vertex_normal(normals[x+delta][z][1], normals[x+delta][z][2], normals[x+delta][z][3])
                mesh_add_vertex(mymesh)
                
                vertex_pos(x, -values[x][z+delta], z + delta)
                vertex_normal(normals[x][z+delta][1], normals[x][z+delta][2], normals[x][z+delta][3])
                mesh_add_vertex(mymesh)
                
                mesh_commit_triangle(mymesh, mat)
            end
            
            if values[x-delta] ~= nil and values[x-delta][z+delta] ~= nil and values[x][z+delta] ~= nil then
                vertex_pos(x, -values[x][z], z)
                vertex_normal(normals[x][z][1], normals[x][z][2], normals[x][z][3])
                mesh_add_vertex(mymesh)
                
                vertex_pos(x, -values[x][z+delta], z + delta)
                vertex_normal(normals[x][z+delta][1], normals[x][z+delta][2], normals[x][z+delta][3])
                mesh_add_vertex(mymesh)
                
                vertex_pos(x - delta, -values[x-delta][z+delta], z + delta)
                vertex_normal(normals[x-delta][z+delta][1], normals[x-delta][z+delta][2], normals[x-delta][z+delta][3])
                mesh_add_vertex(mymesh)
                
                mesh_commit_triangle(mymesh, mat)
            end
        end
    end

    mesh_finalize(mymesh)
    render()
    savebmp("image" .. i .. ".bmp")
end
