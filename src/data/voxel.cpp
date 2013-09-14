#include "data/voxel.h"
#include "client.h"
#include "system/voxel_material_system.h"

namespace trillek {

voxel::voxel()
{
    this->_material=voxel_material_system::get().get_default_nonsolid();
}

voxel::voxel(material_data* _material)
{
    this->_material=_material;
}

voxel::~voxel() {}

bool voxel::is_solid() const
{
    return _material->is_solid();
}

uint16_t voxel::value() const
{
    return _material->get_value();
}

bool operator ==(const voxel& lhs, const voxel& rhs) {
    return lhs.value()==rhs.value();
}
bool operator !=(const voxel& lhs, const voxel& rhs) {
    return !(lhs == rhs);
}

}
