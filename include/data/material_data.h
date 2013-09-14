#ifndef MATERIAL_DATA_H
#define MATERIAL_DATA_H

#include "data/data.h"
#include <stdint.h>

namespace trillek
{

class material_data : public data
{
    public:
        material_data();
        virtual ~material_data();
        bool is_solid();
        uint16_t get_value();
        void set_solid(bool _solid);
        void set_value(uint16_t _value);
    protected:
    private:
        bool solid;
        uint16_t value;
};

}

#endif // MATERIAL_DATA_H
