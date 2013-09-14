#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H
#include <string>
#include "data/data.h"

namespace trillek{

class asset_loader
{
    public:
        asset_loader(){};
        virtual ~asset_loader(){};
        virtual data* load(const std::string& file) const = 0;
        virtual void init()=0;
    protected:
    private:
};

}

#endif // ASSET_LOADER_H
