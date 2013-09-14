#include "data/material_data.h"

namespace trillek
{

bool material_data::is_solid()
{
    return solid;
}

uint16_t material_data::get_value()
{
    return value;
}

void material_data::set_solid(bool _solid)
{
    solid=_solid;
}

void material_data::set_value(uint16_t _value)
{
    value=_value;
}

material_data::material_data()
{
    solid=false;
    value=0;
}

material_data::~material_data()
{
    //dtor
}

}
