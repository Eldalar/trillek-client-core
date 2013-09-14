#ifndef VOXEL_MATERIAL_SYSTEM_H
#define VOXEL_MATERIAL_SYSTEM_H

#include "data/material_data.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace trillek
{

class voxel_material_system
{
    public:
        static voxel_material_system& get();
        virtual ~voxel_material_system();
        material_data* get_default_solid();
        material_data* get_default_nonsolid();
    protected:
    private:
        voxel_material_system();
        voxel_material_system(voxel_material_system const&) = delete;
        void operator=(voxel_material_system const&) = delete;
        std::unordered_map<std::string,std::unique_ptr<material_data>>
                materials;
};

}

#endif // VOXEL_MATERIAL_SYSTEM_H
