#include "data/mesh_data.h"
#include <iostream>
namespace trillek
{

mesh_data::mesh_data()
{
    num_vertices=0;
}

mesh_data::~mesh_data()
{
    //dtor
}

std::map<material_data*,std::vector<vertex_data>>*
    mesh_data::get_vertex_data()
{
    return &vertex_buffer;
}

void mesh_data::reserve(std::size_t new_size)
{
    reserved_space=new_size*3;
}

void mesh_data::add_polygon(vector3d<float> v1,
                            vector3d<float> n1,
                            vector3d<float> v2,
                            vector3d<float> n2,
                            vector3d<float> v3,
                            vector3d<float> n3,
                            material_data* material)
{
    if(vertex_buffer.find(material)==vertex_buffer.end())
    {
        vertex_buffer[material]=std::vector<vertex_data>();
        vertex_buffer[material].reserve(reserved_space);
    }
    vertex_data data;
    data.x =v1.x;data.y =v1.y;data.z =v1.z;
    data.nx=n1.x;data.ny=n1.y;data.nz=n1.z;
    vertex_buffer[material].push_back(data);

    vertex_data data2;
    data2.x =v2.x;data2.y =v2.y;data2.z =v2.z;
    data2.nx=n2.x;data2.ny=n2.y;data2.nz=n2.z;
    vertex_buffer[material].push_back(data2);

    vertex_data data3;
    data3.x =v3.x;data3.y =v3.y;data3.z =v3.z;
    data3.nx=n3.x;data3.ny=n3.y;data3.nz=n3.z;
    vertex_buffer[material].push_back(data3);
}

}
