#ifndef mesh_data_H
#define mesh_data_H

#include "rendering/render_tree.h"
#include <map>
#include "math/vector.h"
#include "data/material_data.h"

namespace trillek
{

struct vertex_data
{
    float x,y,z;        // Vertex
    float nx,ny,nz;     // Normal
    float s0,t0;        // Texture
};

class mesh_data : public render_data
{
    public:
        mesh_data();
        virtual ~mesh_data();
        void add_polygon(vector3d<float> v1,
                         vector3d<float> n1,
                         vector3d<float> v2,
                         vector3d<float> n2,
                         vector3d<float> v3,
                         vector3d<float> n3,
                         material_data* material);
        std::map<material_data*,std::vector<vertex_data>>* get_vertex_data();
        void reserve(std::size_t new_size);
        virtual data_type get_type(){return dt_mesh;}
    protected:
    private:
        std::size_t reserved_space;
        unsigned int num_vertices;
        std::map<material_data*,std::vector<vertex_data>> vertex_buffer;
};

}

#endif // mesh_data_H
