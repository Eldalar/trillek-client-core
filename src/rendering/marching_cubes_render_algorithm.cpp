#include "rendering/marching_cubes_render_algorithm.h"
#include <iostream>
#include "data/voxel_data.h"
#include "math/vector.h"
#include <SFML/OpenGL.hpp>
#include <rendering/marching_cubes_lookup_table.h>
#include <services/graphics_service.h>
#include <array>
#include "client.h"
#include "system/voxel_material_system.h"

namespace trillek
{

marching_cubes_render_algorithm::marching_cubes_render_algorithm()
{
    //ctor
}

marching_cubes_render_algorithm::~marching_cubes_render_algorithm()
{
    //dtor
}

std::map<material_data*,unsigned char> marching_cubes_render_algorithm::
        materials_to_cube_nums(std::array<material_data*,8> m)
{
    std::map<material_data*,unsigned char> retval;

    for(int i=0; i<8;++i)
    {
        if(m[i]->is_solid()&&
           retval.find(m[i])==retval.end())
        {
            std::array<bool,8> v;
            for(int i2=0;i2<8;i2++)
            {
                v[i2]=(m[i]==m[i2]);
            }
            retval[m[i]]=values_to_cube_num(v);
        }
    }

    return retval;
}

unsigned char marching_cubes_render_algorithm::
              values_to_cube_num(bool v0,bool v1,
                                 bool v2,bool v3,
                                 bool v4,bool v5,
                                 bool v6,bool v7)
{
    unsigned char cube_num=0;
    if(v0>0) cube_num |= 1 << 0;
    if(v1>0) cube_num |= 1 << 1;
    if(v2>0) cube_num |= 1 << 4;
    if(v3>0) cube_num |= 1 << 5;
    if(v4>0) cube_num |= 1 << 3;
    if(v5>0) cube_num |= 1 << 2;
    if(v6>0) cube_num |= 1 << 7;
    if(v7>0) cube_num |= 1 << 6;
    return cube_num;
}
unsigned char marching_cubes_render_algorithm::
              values_to_cube_num(std::array<bool,8> values)
{
    return values_to_cube_num(values[0],values[1],values[2],values[3],
                              values[4],values[5],values[6],values[7]);
}

unsigned char marching_cubes_render_algorithm::values_to_cube_num(voxel& v0,
                                                                  voxel& v1,
                                                                  voxel& v2,
                                                                  voxel& v3,
                                                                  voxel& v4,
                                                                  voxel& v5,
                                                                  voxel& v6,
                                                                  voxel& v7)
{
    return values_to_cube_num(v0.is_solid(),v1.is_solid(),
                              v2.is_solid(),v3.is_solid(),
                              v4.is_solid(),v5.is_solid(),
                              v6.is_solid(),v7.is_solid());
}

std::bitset<8> marching_cubes_render_algorithm::cube_num_to_values(
                                                    unsigned char cube_num)
{
    std::bitset<8> retval;
    if(cube_num&0x01)   retval[0]=1;
    if(cube_num&0x02)   retval[1]=1;
    if(cube_num&0x04)   retval[5]=1;
    if(cube_num&0x08)   retval[4]=1;
    if(cube_num&0x10)   retval[2]=1;
    if(cube_num&0x20)   retval[3]=1;
    if(cube_num&0x40)   retval[7]=1;
    if(cube_num&0x80)   retval[6]=1;
    return retval;
}

vector3d<float> marching_cubes_render_algorithm::num_to_offset(
                                                    unsigned char num,
                                                    float negative_val,
                                                    float positive_val)
{
    return vector3d<float>(
    num&0x1?positive_val:negative_val,
    num&0x2?positive_val:negative_val,
    num&0x4?positive_val:negative_val);
}

material_data* get_voxel_material( std::size_t x,std::size_t y,
                            std::size_t z,voxel_data* data)
{
    std::size_t _size=data->get_size().x;
    if(x>_size-1||y>_size-1||z>_size-1)
    {
        return voxel_material_system::get().get_default_nonsolid();
    }else
    {
        return data->get_voxel(x,y,z).get_material();
    }
}

material_data* get_voxel_material(vector3d<std::size_t> pos,voxel_data* data)
{
    return get_voxel_material(pos.x,pos.y,pos.z,data);
}

int16_t get_voxel_value(std::size_t x,std::size_t y,
                        std::size_t z,voxel_data* data)
{
    std::size_t _size=data->get_size().x;
    if(x>_size-1||y>_size-1||z>_size-1)
    {
        return 0;
    }else
    {
        return data->get_voxel(x,y,z).value();
    }
}

int16_t get_voxel_value(vector3d<std::size_t> pos,voxel_data* data)
{
    return get_voxel_value(pos.x,pos.y,pos.z,data);
}

/*
    def defines whether the x,y and z unsigned chars are the values of
    data[n-1] or data[n+1] in the order
    0x1 -> x
    0x2 -> y
    0x4 -> z
    and
    0 -> data[n-1]
    1 -> data[n+1]
*/
vector3d<float> marching_cubes_render_algorithm::calculate_normal(
                                                        std::size_t pos_x,
                                                        std::size_t pos_y,
                                                        std::size_t pos_z,
                                                        std::size_t width,
                                                        std::size_t height,
                                                        std::size_t depth,
                                                         unsigned char x,
                                                         unsigned char y,
                                                         unsigned char z,
                                                         unsigned char def,
                                                         voxel_data* data)
{
    if(!(def&1))
        pos_x+=width;
    if(!(def&2))
        pos_y+=height;
    if(!(def&4))
        pos_z+=depth;
    vector3d<float> retval;
    if(def&1) {
        retval.x= get_voxel_value(pos_x-1,pos_y,pos_z,data)-x;
    }else {
        retval.x= x-get_voxel_value(pos_x+1,pos_y,pos_z,data);
    }
    if(def&2) {
        retval.y= get_voxel_value(pos_x,pos_y-1,pos_z,data)-y;
    }else {
        retval.y= y-get_voxel_value(pos_x,pos_y+1,pos_z,data);
    }
    if(def&4) {
        retval.z= get_voxel_value(pos_x,pos_y,pos_z-1,data)-z;
    }else {
        retval.z= z-get_voxel_value(pos_x,pos_y,pos_z+1,data);
    }
    return retval;
}

void marching_cubes_render_algorithm::step( vector3d<float> p0,
                                            vector3d<float> p1,
                                            vector3d<float> p2,
                                            vector3d<float> p3,
                                            vector3d<float> p4,
                                            vector3d<float> p5,
                                            vector3d<float> p6,
                                            vector3d<float> p7,
                                            unsigned char cubeindex,
                                            std::shared_ptr<mesh_data> model,
                                            voxel_data* data,
                                            material_data* material)
{
    std::bitset<8> v = cube_num_to_values(cubeindex);

    // The following simply checks whether or not a certain edge is present in
    // the resulting polygons and makes sure that only those actually present
    // have to be calculated
    std::array<vector3d<float>,12> edge_vertex_table;
    std::array<vector3d<float>,12> edge_gradient_table;
    std::size_t edge_index=edge_table[cubeindex];
    std::size_t x=p0.x;
    std::size_t y=p0.y;
    std::size_t z=p0.z;
    vector3d<float> diff=p7-p0;
    std::size_t width =diff.x;
    std::size_t height=diff.y;
    std::size_t depth =diff.z;
    if(edge_index&0x01) {
        edge_gradient_table[ 0]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[1],v[2],v[4],0x7,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[0],v[3],v[5],0x6,data)).normalize();
        edge_vertex_table[ 0]=interpolate(p0,p1);
    }
    if(edge_index&0x02) {
        edge_gradient_table[ 1]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[0],v[3],v[5],0x6,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[4],v[7],v[1],0x2,data)).normalize();
        edge_vertex_table[ 1]=interpolate(p1,p5);
    }
    if(edge_index&0x04) {
        edge_gradient_table[ 2]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[4],v[7],v[1],0x2,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[5],v[6],v[0],0x3,data)).normalize();
        edge_vertex_table[ 2]=interpolate(p5,p4);
    }
    if(edge_index&0x08) {
        edge_gradient_table[ 3]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[5],v[6],v[0],0x3,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[1],v[2],v[4],0x7,data)).normalize();
        edge_vertex_table[ 3]=interpolate(p4,p0);
    }
    if(edge_index&0x10) {
        edge_gradient_table[ 4]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[3],v[0],v[6],0x5,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[2],v[1],v[7],0x4,data)).normalize();
        edge_vertex_table[ 4]=interpolate(p2,p3);
    }
    if(edge_index&0x20) {
        edge_gradient_table[ 5]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[2],v[1],v[7],0x4,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[6],v[5],v[3],0x0,data)).normalize();
        edge_vertex_table[ 5]=interpolate(p3,p7);
    }
    if(edge_index&0x40) {
        edge_gradient_table[ 6]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[6],v[5],v[3],0x0,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[7],v[4],v[2],0x1,data)).normalize();
        edge_vertex_table[ 6]=interpolate(p7,p6);
    }
    if(edge_index&0x80) {
        edge_gradient_table[ 7]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[7],v[4],v[2],0x1,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[3],v[0],v[6],0x5,data)).normalize();
        edge_vertex_table[ 7]=interpolate(p6,p2);
    }
    if(edge_index&0x100) {
        edge_gradient_table[ 8]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[3],v[0],v[6],0x5,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[1],v[2],v[4],0x7,data)).normalize();
        edge_vertex_table[ 8]=interpolate(p2,p0);
    }
    if(edge_index&0x200) {
        edge_gradient_table[ 9]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[2],v[1],v[7],0x4,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[0],v[3],v[5],0x6,data)).normalize();
        edge_vertex_table[ 9]=interpolate(p3,p1);
    }
    if(edge_index&0x400) {
        edge_gradient_table[10]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[6],v[5],v[3],0x0,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[4],v[7],v[1],0x2,data)).normalize();
        edge_vertex_table[10]=interpolate(p7,p5);
    }
    if(edge_index&0x800) {
        edge_gradient_table[11]=interpolate(
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[7],v[4],v[2],0x1,data),
            calculate_normal(x,y,z,
                               width,height,depth,
                               v[5],v[6],v[0],0x3,data)).normalize();
        edge_vertex_table[11]=interpolate(p6,p4);
    }
    for (int n=0; tritable[cubeindex][n] != -1; n+=3)
    {
        model->add_polygon( edge_vertex_table[tritable[cubeindex][n+0]],
                            edge_gradient_table[tritable[cubeindex][n+0]],
                            edge_vertex_table[tritable[cubeindex][n+1]],
                            edge_gradient_table[tritable[cubeindex][n+1]],
                            edge_vertex_table[tritable[cubeindex][n+2]],
                            edge_gradient_table[tritable[cubeindex][n+2]],
                            material);
    }
}

void marching_cubes_render_algorithm::step( axis_aligned_box& box,
                                            unsigned char cubeindex,
                                            std::shared_ptr<mesh_data> model,
                                            voxel_data* data,
                                            material_data* material)
{
    step(box.get_corner(0),box.get_corner(1),
         box.get_corner(2),box.get_corner(3),
         box.get_corner(4),box.get_corner(5),
         box.get_corner(6),box.get_corner(7),
         cubeindex, model,data,material);
}

void marching_cubes_render_algorithm::step( vector3d<float> pos,
                                            unsigned char cubeindex,
                                            float size,
                                            std::shared_ptr<mesh_data> model,
                                            voxel_data* data,
                                            material_data* material)
{
        vector3d<float> p0=pos + num_to_offset(0,-size/2,size/2);
        vector3d<float> p1=pos + num_to_offset(1,-size/2,size/2);
        vector3d<float> p2=pos + num_to_offset(2,-size/2,size/2);
        vector3d<float> p3=pos + num_to_offset(3,-size/2,size/2);
        vector3d<float> p4=pos + num_to_offset(4,-size/2,size/2);
        vector3d<float> p5=pos + num_to_offset(5,-size/2,size/2);
        vector3d<float> p6=pos + num_to_offset(6,-size/2,size/2);
        vector3d<float> p7=pos + num_to_offset(7,-size/2,size/2);
        step(p0,p1,p2,p3,
             p4,p5,p6,p7,
             cubeindex,model,data,material);
}

void marching_cubes_render_algorithm::step(vector3d<float> pos,
                                          std::shared_ptr<mesh_data> model,
                                          voxel_data* data)
{
    std::array<material_data*,8> n;
    for(unsigned char i=0; i<8;++i)
    {
        n[i]=get_voxel_material(pos+num_to_offset(i,0,1),data);
    }
    std::map<material_data*,unsigned char> cube_nums = materials_to_cube_nums(n);

    for(auto& cube_num : cube_nums)
    {

        if(cube_num.second==0||cube_num.second==0xFF)
            return;
        marching_cubes_render_algorithm::step(pos+vector3d<float>(0.5f,0.5f,0.5f),
                                              cube_num.second,1.0f,model,data,
                                              cube_num.first);
    }
}

void marching_cubes_render_algorithm::process(voxel_model* node,
                                              std::shared_ptr<mesh_data> model)
{
    voxel_data* data=node->get_render_data();
    voxel_data::size_vector3d size=data->get_size();
    model->reserve(size.x*size.y*size.z);
    for(int x=-1;x<static_cast<int>(size.x);++x)
    {
        for(int y=-1;y<static_cast<int>(size.y);++y)
        {
            for(int z=-1;z<static_cast<int>(size.z);++z)
            {
                vector3d<float> pos(x,y,z);
                step(pos,model,data);
            }
        }
    }
}

void marching_cubes_render_algorithm::process(voxel_model* node,
                                              graphics_service* service)
{
    std::shared_ptr<mesh_data> model = std::make_shared<mesh_data>();
    process(node,model);
    service->register_model((uintptr_t)node,model);
}

}
