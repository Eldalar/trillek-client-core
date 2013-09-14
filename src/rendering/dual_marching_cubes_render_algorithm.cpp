#include "rendering/dual_marching_cubes_render_algorithm.h"
#include "rendering/marching_cubes_render_algorithm.h"
#include "services/graphics_service.h"
#include <SFML/OpenGL.hpp>
#include <algorithm>
#include "math/axis_aligned_box.h"
#include "general/count_unique_params.h"
#include "general/min_max.h"
#include "client.h"
#include "system/voxel_material_system.h"

namespace trillek
{

dual_marching_cubes_render_algorithm::dual_marching_cubes_render_algorithm()
{
    //ctor
}

dual_marching_cubes_render_algorithm::~dual_marching_cubes_render_algorithm()
{
    //dtor
}

template <typename... Args>
auto values_to_cube_num(Args&&... args) ->
decltype(marching_cubes_render_algorithm::values_to_cube_num(
                std::forward<Args>(args)...))
{
  return marching_cubes_render_algorithm::values_to_cube_num(
        std::forward<Args>(args)...);
}

template <typename... T>
std::size_t process_dual_cell_box(std::size_t cube_num,
                            std::shared_ptr<mesh_data> model,
                            axis_aligned_box& center_box,
                            std::size_t type,
                            voxel_data* data,
                            T... n)
{
    float size=1.0f;
    static_assert(are_same<voxel_data*,T..>::value,
        "Tried to call process_duall_cell_box with a \
        type other then voxel_octree*");
    std::size_t min_size=min((n?n->get_size().x:2000)...);
    if(min_size>size)
    {
        axis_aligned_box face_box=center_box;
        face_box.set_widths(type&0x1?(float)min_size-size:size,
                            type&0x2?(float)min_size-size:size,
                            type&0x4?(float)min_size-size:size);
        face_box.translate(type&0x1?min_size/2.0f:0,
                            type&0x2?min_size/2.0f:0,
                            type&0x4?min_size/2.0f:0);
        marching_cubes_render_algorithm::step(face_box,cube_num,model,data,
          voxel_material_system::get().get_default_solid());
    }
    return min_size;
}

std::size_t process_dual_cell_face(voxel_octree* n0,
                            voxel_octree* n1,
                            std::size_t cube_num,
                            std::shared_ptr<mesh_data> model,
                            axis_aligned_box& center_box,
                            std::size_t type,
                            voxel_data* data)
{
    if(cube_num!=0&&cube_num!=255)
    {
        if(count_unique_params(n0,n1)>=2)
        {
            return process_dual_cell_box(cube_num,model,
                                         center_box,type,data,
                                         n0,n1);
        }
    }
    return 0;
}

std::size_t process_dual_cell_edge(voxel_octree* n0,
                                   voxel_octree* n1,
                                   voxel_octree* n2,
                                   voxel_octree* n3,
                                   std::size_t cube_num,
                                   std::shared_ptr<mesh_data> model,
                                   axis_aligned_box& center_box,
                                   std::size_t type,
                                   voxel_data* data)
{
    if(cube_num!=0&&cube_num!=255)
    {
        if(count_unique_params(n0,n1,n2,n3)>2)
        {
            return process_dual_cell_box(cube_num,model,
                                         center_box,type,data,
                                         n0,n1,n2,n3);
        }
    }
    return 0;
}
void dual_marching_cubes_render_algorithm::handle_borders(
                                            std::shared_ptr<mesh_data> model,
                                            voxel_data* data)
{
    vector3d<float> size=data->get_size();
    for(int x=-1;x<static_cast<int>(size.x);++x)
    {
        for(int y=-1;y<static_cast<int>(size.y);++y)
        {
            marching_cubes_render_algorithm::step(
                            vector3d<float>(x,y,-1),
                            model,data);
            marching_cubes_render_algorithm::step(
                            vector3d<float>(x,y,static_cast<int>(size.x)-1),
                            model,data);
            marching_cubes_render_algorithm::step(
                            vector3d<float>(x,-1,y),
                            model,data);
            marching_cubes_render_algorithm::step(
                            vector3d<float>(x,static_cast<int>(size.x)-1,y),
                            model,data);
            marching_cubes_render_algorithm::step(
                            vector3d<float>(-1,x,y),
                            model,data);
            marching_cubes_render_algorithm::step(
                            vector3d<float>(static_cast<int>(size.x)-1,x,y),
                            model,data);
        }
    }
}

void dual_marching_cubes_render_algorithm::create_dual_cells(voxel_octree* n0,
                                                             voxel_octree* n1,
                                                             voxel_octree* n2,
                                                             voxel_octree* n3,
                                                             voxel_octree* n4,
                                                             voxel_octree* n5,
                                                             voxel_octree* n6,
                                                             voxel_octree* n7,
                                             std::shared_ptr<mesh_data> model,
                                                             voxel_data* data)
{
    voxel nonsolid=
    voxel_material_system::get().get_default_nonsolid();
    std::array<voxel,8> v = {{n0?n0->get_voxel():nonsolid,
                             n1?n1->get_voxel():nonsolid,
                             n2?n2->get_voxel():nonsolid,
                             n3?n3->get_voxel():nonsolid,
                             n4?n4->get_voxel():nonsolid,
                             n5?n5->get_voxel():nonsolid,
                             n6?n6->get_voxel():nonsolid,
                             n7?n7->get_voxel():nonsolid}};
    std::array<voxel_octree*,8> n={{n0,n1,n2,n3,n4,n5,n6,n7}};
    // First the cube_num is calculated, since we can skip the entire rest of
    // this function if the cube is entirely full(255) or empty(0)
    std::size_t cube_num= values_to_cube_num(v[0],v[1],v[2],v[3],
                                             v[4],v[5],v[6],v[7]);
    if(cube_num==0||cube_num==255)
        return;

    // The cube is being intersected by the surface, so now we calculate which edge of
    // the cube is touched by all 8 voxel_octrees (they have to be nodes)
    // For this we find the smallest cube
    // and while we are at it we also find the biggest size, which will be needed later
    std::size_t min_size_num=0;
	std::size_t min_size=std::numeric_limits<std::size_t>::max();
	std::size_t max_size=0;
    for(int i=0; i<8;++i) {
        if(n[i]->get_size().x<min_size) {
            min_size=n[i]->get_size().x;
            min_size_num=i;
        }
        if(n[i]->get_size().x>max_size) {
            max_size=n[i]->get_size().x;
        }
    }
    float size=1.0f;

    // Since we now have the smallest cube we can calculate the corner that all the
    // nodes share
    auto new_offset=n[min_size_num]->get_child_offset_by_index(min_size_num);
    auto new_center =n[min_size_num]->get_offset()
                     -new_offset*2
                     -vector3d<float>(size/2,size/2,size/2);

    // Which we then make into an axis_aligned_box,
    // with the minimum (1.0) width, height and depth
    axis_aligned_box center_box(new_center,vector3d<std::size_t>(size,
                                                                 size,
                                                                 size));
    marching_cubes_render_algorithm::step(center_box,cube_num,model,data,
          voxel_material_system::get().get_default_solid());

    // Edges and Faces only need to be computed if that corner is not
    // completely filled by the box above, as in one of the nodes at
    // that corner has a size bigger then the minimum possible size(1.0)
    if(max_size>size) {
        // Now we compute the 3 positive axes and keep their length

        // X-Axis
        std::size_t min_size_x=process_dual_cell_edge(
                                            n[1],n[3],n[5],n[7],
                                            values_to_cube_num(v[1],v[1],
                                                               v[3],v[3],
                                                               v[5],v[5],
                                                               v[7],v[7]),
                                            model,center_box,0x1,data);

        // Y-Axis
        std::size_t min_size_y=process_dual_cell_edge(
                                            n[2],n[3],n[6],n[7],
                                            values_to_cube_num(v[2],v[3],
                                                               v[2],v[3],
                                                               v[6],v[7],
                                                               v[6],v[7]),
                                            model,center_box,0x2,data);

        // Z-Axis
        std::size_t min_size_z=process_dual_cell_edge(
                                            n[4],n[5],n[6],n[7],
                                            values_to_cube_num(v[4],v[5],
                                                               v[6],v[7],
                                                               v[4],v[5],
                                                               v[6],v[7]),
                                            model,center_box,0x4,data);

        // And then we compute the 3 faces between the 3 positive axes above
        // they are only drawn if both axes have a length > 0 since otherwise
        // one of their lengths would be 0 as well, resulting in unneded
        // processing and drawing
        // XY-Face
        if(min_size_x != 0 && min_size_y != 0) {
            cube_num=values_to_cube_num(v[3],v[3],v[3],v[3],v[7],v[7],v[7],v[7]);
            process_dual_cell_face(n[3],n[7],cube_num,model,center_box,0x3,data);
        }

        // YZ-Face
        if(min_size_y != 0 && min_size_z != 0) {
            cube_num=values_to_cube_num(v[6],v[7],v[6],v[7],v[6],v[7],v[6],v[7]);
            process_dual_cell_face(n[6],n[7],cube_num,model,center_box,0x6,data);
        }

        // XZ-Face
        if(min_size_x != 0 && min_size_z != 0) {
            cube_num=values_to_cube_num(v[5],v[5],v[7],v[7],v[5],v[5],v[7],v[7]);
            process_dual_cell_face(n[5],n[7],cube_num,model,center_box,0x5,data);
        }
    }
}

void dual_marching_cubes_render_algorithm::vertProc(voxel_octree* n0,
                                                    voxel_octree* n1,
                                                    voxel_octree* n2,
                                                    voxel_octree* n3,
                                                    voxel_octree* n4,
                                                    voxel_octree* n5,
                                                    voxel_octree* n6,
                                                    voxel_octree* n7,
                                            std::shared_ptr<mesh_data> model,
                                                    voxel_data* data)
{
    if(n0->has_children()||n1->has_children()||
       n2->has_children()||n3->has_children()||
       n4->has_children()||n5->has_children()||
       n6->has_children()||n7->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c1 = n1->has_children() ? n1->get_child(0,1,1):n1;
        voxel_octree* c2 = n2->has_children() ? n2->get_child(1,0,1):n2;
        voxel_octree* c3 = n3->has_children() ? n3->get_child(0,0,1):n3;
        voxel_octree* c4 = n4->has_children() ? n4->get_child(1,1,0):n4;
        voxel_octree* c5 = n5->has_children() ? n5->get_child(0,1,0):n5;
        voxel_octree* c6 = n6->has_children() ? n6->get_child(1,0,0):n6;
        voxel_octree* c7 = n7->has_children() ? n7->get_child(0,0,0):n7;
        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }else
    {
        create_dual_cells(n0,n1,n2,n3,n4,n5,n6,n7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::edgeProcX( voxel_octree* n0,
                                                      voxel_octree* n1,
                                                      voxel_octree* n2,
                                                      voxel_octree* n3,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children()||
       n2->has_children()||n3->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(0,1,1):n0;
        voxel_octree* c1 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c2 = n1->has_children() ? n1->get_child(0,0,1):n1;
        voxel_octree* c3 = n1->has_children() ? n1->get_child(1,0,1):n1;
        voxel_octree* c4 = n2->has_children() ? n2->get_child(0,1,0):n2;
        voxel_octree* c5 = n2->has_children() ? n2->get_child(1,1,0):n2;
        voxel_octree* c6 = n3->has_children() ? n3->get_child(0,0,0):n3;
        voxel_octree* c7 = n3->has_children() ? n3->get_child(1,0,0):n3;

        edgeProcX(c0,c2,c4,c6,model,data);
        edgeProcX(c1,c3,c5,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::edgeProcY( voxel_octree* n0,
                                                      voxel_octree* n1,
                                                      voxel_octree* n2,
                                                      voxel_octree* n3,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children()||
       n2->has_children()||n3->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(1,0,1):n0;
        voxel_octree* c1 = n1->has_children() ? n1->get_child(0,0,1):n1;
        voxel_octree* c2 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c3 = n1->has_children() ? n1->get_child(0,1,1):n1;
        voxel_octree* c4 = n2->has_children() ? n2->get_child(1,0,0):n2;
        voxel_octree* c5 = n3->has_children() ? n3->get_child(0,0,0):n3;
        voxel_octree* c6 = n2->has_children() ? n2->get_child(1,1,0):n2;
        voxel_octree* c7 = n3->has_children() ? n3->get_child(0,1,0):n3;

        edgeProcY(c0,c1,c4,c5,model,data);
        edgeProcY(c2,c3,c6,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::edgeProcZ( voxel_octree* n0,
                                                      voxel_octree* n1,
                                                      voxel_octree* n2,
                                                      voxel_octree* n3,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children()||
       n2->has_children()||n3->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(1,1,0):n0;
        voxel_octree* c1 = n1->has_children() ? n1->get_child(0,1,0):n1;
        voxel_octree* c2 = n2->has_children() ? n2->get_child(1,0,0):n2;
        voxel_octree* c3 = n3->has_children() ? n3->get_child(0,0,0):n3;
        voxel_octree* c4 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c5 = n1->has_children() ? n1->get_child(0,1,1):n1;
        voxel_octree* c6 = n2->has_children() ? n2->get_child(1,0,1):n2;
        voxel_octree* c7 = n3->has_children() ? n3->get_child(0,0,1):n3;

        edgeProcZ(c0,c1,c2,c3,model,data);
        edgeProcZ(c4,c5,c6,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::faceProcXY(voxel_octree* n0,
                                                      voxel_octree* n1,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(0,0,1):n0;
        voxel_octree* c1 = n0->has_children() ? n0->get_child(1,0,1):n0;
        voxel_octree* c2 = n0->has_children() ? n0->get_child(0,1,1):n0;
        voxel_octree* c3 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c4 = n1->has_children() ? n1->get_child(0,0,0):n1;
        voxel_octree* c5 = n1->has_children() ? n1->get_child(1,0,0):n1;
        voxel_octree* c6 = n1->has_children() ? n1->get_child(0,1,0):n1;
        voxel_octree* c7 = n1->has_children() ? n1->get_child(1,1,0):n1;

        faceProcXY(c0,c4,model,data);
        faceProcXY(c1,c5,model,data);
        faceProcXY(c2,c6,model,data);
        faceProcXY(c3,c7,model,data);

        edgeProcX(c0,c2,c4,c6,model,data);
        edgeProcX(c1,c3,c5,c7,model,data);

        edgeProcY(c0,c1,c4,c5,model,data);
        edgeProcY(c2,c3,c6,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::faceProcYZ(voxel_octree* n0,
                                                      voxel_octree* n1,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(1,0,0):n0;
        voxel_octree* c1 = n1->has_children() ? n1->get_child(0,0,0):n1;
        voxel_octree* c2 = n0->has_children() ? n0->get_child(1,1,0):n0;
        voxel_octree* c3 = n1->has_children() ? n1->get_child(0,1,0):n1;
        voxel_octree* c4 = n0->has_children() ? n0->get_child(1,0,1):n0;
        voxel_octree* c5 = n1->has_children() ? n1->get_child(0,0,1):n1;
        voxel_octree* c6 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c7 = n1->has_children() ? n1->get_child(0,1,1):n1;

        faceProcYZ(c0,c1,model,data);
        faceProcYZ(c2,c3,model,data);
        faceProcYZ(c4,c5,model,data);
        faceProcYZ(c6,c7,model,data);

        edgeProcY(c0,c1,c4,c5,model,data);
        edgeProcY(c2,c3,c6,c7,model,data);

        edgeProcZ(c0,c1,c2,c3,model,data);
        edgeProcZ(c4,c5,c6,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::faceProcXZ(voxel_octree* n0,
                                                      voxel_octree* n1,
                                            std::shared_ptr<mesh_data> model,
                                                      voxel_data* data)
{
    if(n0->has_children()||n1->has_children())
    {
        voxel_octree* c0 = n0->has_children() ? n0->get_child(0,1,0):n0;
        voxel_octree* c1 = n0->has_children() ? n0->get_child(1,1,0):n0;
        voxel_octree* c2 = n1->has_children() ? n1->get_child(0,0,0):n1;
        voxel_octree* c3 = n1->has_children() ? n1->get_child(1,0,0):n1;
        voxel_octree* c4 = n0->has_children() ? n0->get_child(0,1,1):n0;
        voxel_octree* c5 = n0->has_children() ? n0->get_child(1,1,1):n0;
        voxel_octree* c6 = n1->has_children() ? n1->get_child(0,0,1):n1;
        voxel_octree* c7 = n1->has_children() ? n1->get_child(1,0,1):n1;

        faceProcXZ(c0,c2,model,data);
        faceProcXZ(c1,c3,model,data);
        faceProcXZ(c4,c6,model,data);
        faceProcXZ(c5,c7,model,data);

        edgeProcX(c0,c2,c4,c6,model,data);
        edgeProcX(c1,c3,c5,c7,model,data);

        edgeProcZ(c0,c1,c2,c3,model,data);
        edgeProcZ(c4,c5,c6,c7,model,data);

        vertProc(c0,c1,c2,c3,c4,c5,c6,c7,model,data);
    }
}

void dual_marching_cubes_render_algorithm::_nodeProc(voxel_octree* n,
                                            std::shared_ptr<mesh_data> model,
                                                    voxel_data* data)
{
    if(n->has_children())
    {
        // Visit all eight children
        for(int i=0; i<8; ++i)
            _nodeProc(n->get_child(i),model,data);

        faceProcXY(n->get_child(0,0,0),n->get_child(0,0,1),model,data);
        faceProcXY(n->get_child(1,0,0),n->get_child(1,0,1),model,data);
        faceProcXY(n->get_child(0,1,0),n->get_child(0,1,1),model,data);
        faceProcXY(n->get_child(1,1,0),n->get_child(1,1,1),model,data);

        faceProcYZ(n->get_child(0,0,0),n->get_child(1,0,0),model,data);
        faceProcYZ(n->get_child(0,0,1),n->get_child(1,0,1),model,data);
        faceProcYZ(n->get_child(0,1,0),n->get_child(1,1,0),model,data);
        faceProcYZ(n->get_child(0,1,1),n->get_child(1,1,1),model,data);

        faceProcXZ(n->get_child(0,0,0),n->get_child(0,1,0),model,data);
        faceProcXZ(n->get_child(1,0,0),n->get_child(1,1,0),model,data);
        faceProcXZ(n->get_child(0,0,1),n->get_child(0,1,1),model,data);
        faceProcXZ(n->get_child(1,0,1),n->get_child(1,1,1),model,data);

        edgeProcX(n->get_child(0,0,0),n->get_child(0,1,0),
                  n->get_child(0,0,1),n->get_child(0,1,1),model,data);
        edgeProcX(n->get_child(1,0,0),n->get_child(1,1,0),
                  n->get_child(1,0,1),n->get_child(1,1,1),model,data);

        edgeProcY(n->get_child(0,0,0),n->get_child(1,0,0),
                  n->get_child(0,0,1),n->get_child(1,0,1),model,data);
        edgeProcY(n->get_child(0,1,0),n->get_child(1,1,0),
                  n->get_child(0,1,1),n->get_child(1,1,1),model,data);

        edgeProcZ(n->get_child(0,0,0),n->get_child(1,0,0),
                  n->get_child(0,1,0),n->get_child(1,1,0),model,data);
        edgeProcZ(n->get_child(0,0,1),n->get_child(1,0,1),
                  n->get_child(0,1,1),n->get_child(1,1,1),model,data);

        vertProc(n->get_child(0,0,0),n->get_child(1,0,0),
                 n->get_child(0,1,0),n->get_child(1,1,0),
                 n->get_child(0,0,1),n->get_child(1,0,1),
                 n->get_child(0,1,1),n->get_child(1,1,1),model,data);
    }
}

void dual_marching_cubes_render_algorithm::nodeProc(voxel_octree* n,
                                            std::shared_ptr<mesh_data> model,
                                                    voxel_data* data)
{
    if(n->has_children())
        _nodeProc(n,model,data);
    else
        vertProc(n,n,n,n,n,n,n,n,model,data);
}

void dual_marching_cubes_render_algorithm::process(voxel_model* node,graphics_service* service)
{
    std::cerr << "Rendering start" << std::endl;
    std::shared_ptr<mesh_data> model = std::make_shared<mesh_data>();

    voxel_data* r_d = node->get_render_data();

    std::cerr << "Octree-conversion start" << std::endl;
    voxel_octree* octree = voxel_octree::convert(r_d);
    std::cerr << "Octree-conversion end" << std::endl;
    nodeProc(octree,model,r_d);
    handle_borders(model,r_d);
    service->register_model((uintptr_t)node,model);
    std::cerr << "Rendering end" << std::endl;
}

}
