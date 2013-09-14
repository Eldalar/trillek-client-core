#include "system/voxel_material_system.h"
#include "make_unique.h"

namespace trillek
{

voxel_material_system& voxel_material_system::get()
{
    static voxel_material_system instance;
    return instance;
}

voxel_material_system::voxel_material_system()
{
    materials["iron"] = make_unique<material_data>();
    materials["iron"]->set_solid(true);
    materials["iron"]->set_value(1);
    materials["gas"] = make_unique<material_data>();
    materials["gas"]->set_solid(false);
    materials["gas"]->set_value(0);
}

voxel_material_system::~voxel_material_system()
{
    //dtor
}

material_data* voxel_material_system::get_default_solid()
{
    return materials["iron"].get();
}

material_data* voxel_material_system::get_default_nonsolid()
{
    return materials["gas"].get();
}

}
