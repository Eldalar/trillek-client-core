#ifndef marching_cubes_render_algorithm_H
#define marching_cubes_render_algorithm_H

#include "rendering/render_algorithm.h"
#include "data/mesh_data.h"
#include "rendering/voxel_model.h"
#include "math/axis_aligned_box.h"
#include <bitset>

namespace trillek
{

class marching_cubes_render_algorithm
    : public render_algorithm
{
    public:
        marching_cubes_render_algorithm();
        virtual ~marching_cubes_render_algorithm();
        void process(voxel_model* node, graphics_service* service);
        static std::map<material_data*,unsigned char> materials_to_cube_nums(
                                        std::array<material_data*,8> m);
        static unsigned char values_to_cube_num(bool v0,bool v1,
                                                bool v2,bool v3,
                                                bool v4,bool v5,
                                                bool v6,bool v7);
        static unsigned char values_to_cube_num(std::array<bool,8> values);
        static unsigned char values_to_cube_num(voxel& v0,voxel& v1,
                                                voxel& v2,voxel& v3,
                                                voxel& v4,voxel& v5,
                                                voxel& v6,voxel& v7);
        static std::bitset<8> cube_num_to_values(unsigned char cube_num);
        static void step(vector3d<float> pos,
                         std::shared_ptr<mesh_data> model,
                         voxel_data* data);
        static void step(vector3d<float> pos,
                         unsigned char num,
                         float size,
                         std::shared_ptr<mesh_data> model,
                         voxel_data* data,
                         material_data* material);
        static void step(axis_aligned_box& box,
                         unsigned char num,
                         std::shared_ptr<mesh_data> model,
                         voxel_data* data,
                         material_data* material);
        static void step(vector3d<float> p0,vector3d<float> p1,
                         vector3d<float> p2,vector3d<float> p3,
                         vector3d<float> p4,vector3d<float> p5,
                         vector3d<float> p6,vector3d<float> p7,
                         unsigned char cubeindex,
                         std::shared_ptr<mesh_data> model,
                         voxel_data* data,
                        material_data* material);
    protected:
    private:
        static vector3d<float> num_to_offset(unsigned char num,
                                             float positive_val,
                                             float negative_val);
        void process(voxel_model* node, std::shared_ptr<mesh_data> model);
        static vector3d<float> calculate_normal(std::size_t pos_x,
                                                  std::size_t pos_y,
                                                  std::size_t pos_z,
                                                  std::size_t width,
                                                  std::size_t height,
                                                  std::size_t depth,
                                                  unsigned char x,
                                                  unsigned char y,
                                                  unsigned char z,
                                                  unsigned char def,
                                                  voxel_data* data);
};

}

#endif // marching_cubes_render_algorithm_H
