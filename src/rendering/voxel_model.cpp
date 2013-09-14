#include "rendering/voxel_model.h"
#include "data/voxel_array.h"

namespace trillek
{

voxel_model::voxel_model(std::size_t x, std::size_t y, std::size_t z)
        : data(std::make_shared<voxel_array_alternate>(x,y,z)) {}

voxel_model::~voxel_model() {}

void voxel_model::set_voxel(std::size_t x,
                            std::size_t y,
                            std::size_t z,
                            voxel v)
{
    std::dynamic_pointer_cast<voxel_data>(this->data)->set_voxel(x,y,z,v);
}

}
