#ifndef VOXEL_OCTREE_H
#define VOXEL_OCTREE_H

#include "data/voxel_data.h"
#include <memory>
#include <array>

namespace trillek
{

class voxel_octree : public voxel_data
{
public:
    using voxel_data::size_vector3d;
    using voxel_data::get_voxel;
    using voxel_data::set_voxel;
    voxel_octree();
    voxel_octree(const voxel_octree& other);
    voxel_octree(voxel_octree&& other);
    voxel_octree(voxel_data* other);
    virtual ~voxel_octree();
    const voxel& get_voxel(std::size_t x,
                    std::size_t y,
                    std::size_t z) const override;
    const voxel& get_voxel() const;
    void set_voxel(std::size_t x,
                   std::size_t y,
                   std::size_t z,
                   const voxel& v) override;
    vector3d<float> get_child_offset_by_index(unsigned char index);
    vector3d<float> get_offset(){return _offset;}
    size_vector3d get_size() const override;
    data_type get_type() const override {return dt_voxel_octree;}
    /**
     * @brief Get the number of elements in the whole tree
     */
    std::size_t get_num_nodes() const;
    /**
     * @brief Gets the volume of opaque voxels.
     * As though all data was represented by 1x1x1 voxels
     */
    std::size_t get_opaque_volume() const;
    /**
     * @brief Become at least as large as x,y,z
     * Final size is smallest power-of-two cube larger than
     * the maximum of x,y,z.
     * It is not valid to do this operation on an octree with children
     * Doing it on a child of another octree produces undefined behaviour
     * @param x
     * @param y
     * @param z
     */
    void reserve_space(std::size_t x,
                       std::size_t y,
                       std::size_t z);
    void reserve_space(const size_vector3d& xyz);
    static voxel_octree* convert(voxel_data* data);

    bool has_children(){return _has_children;}
    voxel_octree* get_child(std::size_t n);
    voxel_octree* get_child(std::size_t x, std::size_t y, std::size_t z);
protected:
private:
    ///@brief construct an octree with this size having data
    explicit voxel_octree(std::size_t size_exp, const voxel& data,
                          vector3d<float> offset);
    typedef std::unique_ptr<voxel_octree> voxel_octree_ptr;
    typedef std::array<voxel_octree_ptr, 8> voxel_child_array;
    std::size_t compute_child_index(std::size_t x,
                                    std::size_t y,
                                    std::size_t z) const;
    static std::size_t compute_child_index(std::size_t x,
                                           std::size_t y,
                                           std::size_t z,
                                           std::size_t generation);
    size_vector3d compute_child_relative_coordinates(std::size_t x,
                                                     std::size_t y,
                                                     std::size_t z) const;
    /**
     * @brief If I have no children, make children exactly like me.
     * If I do, this function is an error
     */
    void split_children();
    /**
     * @brief If all children are same, delete them and store one value here.
     * Otherwise set value to average of children
     */
    void combine_children();
    //size = (1 << size_exp) or equivalently 2^size_exp
    std::size_t _size_exp;
    voxel _data;
    voxel_child_array _children;
    bool _has_children;
    vector3d<float> _offset;
};

}

#endif // VOXEL_OCTREE_H
