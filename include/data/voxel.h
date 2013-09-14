#ifndef TRILLEK_VOXEL_H
#define TRILLEK_VOXEL_H

#include <cstddef>
#include <stdint.h>
#include "data/material_data.h"

namespace trillek
{

class voxel {
public:
    voxel();
    voxel(material_data* material);
    //voxel(bool a, bool b):standard(a),opaque(b){};
    virtual ~voxel();

    bool is_solid() const;
    uint16_t value() const;
    /**
     * Average up the voxels in this range
     * @param begin
     * @param end
     * @return a voxel that represents the majority
     */
    template <typename ITER_T>
    static voxel average_voxel_range(ITER_T begin, ITER_T end) {
        std::size_t num_standard = 0;
        std::size_t num_opaque = 0;
        std::size_t num_total = 0;
        for(; begin != end; ++begin) {
            ++num_total;
            if(begin->is_standard()) ++num_standard;
            if(begin->is_opaque()) ++num_opaque;
        }
        const std::size_t half_total = num_total / 2;
        return voxel(num_standard > half_total,
                num_opaque > half_total);
    }
    material_data* get_material() const{return _material;}
protected:
private:
    material_data* _material;
};

bool operator ==(const voxel& lhs, const voxel& rhs);
bool operator !=(const voxel& lhs, const voxel& rhs);

}

#endif // TRILLEK_VOXEL_H
