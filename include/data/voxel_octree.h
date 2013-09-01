#ifndef VOXEL_OCTREE_H
#define VOXEL_OCTREE_H

#include "data/voxel_data.h"

namespace trillek
{

class voxel_octree : public voxel_data
{
    public:
        voxel_octree();
        virtual ~voxel_octree();
        vector3d<std::size_t> get_size() const;
        voxel get_voxel(std::size_t x,
                        std::size_t y,
                        std::size_t z) const;
        void set_voxel(std::size_t x,
                       std::size_t y,
                       std::size_t z,
                       voxel v);
        data_type get_type(){return dt_voxel_octree;}
        void reserve_space(std::size_t,std::size_t,std::size_t);
    protected:
    private:
        std::size_t size;
};

}

#endif // VOXEL_OCTREE_H
