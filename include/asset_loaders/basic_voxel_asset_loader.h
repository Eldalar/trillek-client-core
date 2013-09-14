#ifndef BASIC_VOXEL_ASSET_LOADER_H
#define BASIC_VOXEL_ASSET_LOADER_H
#include "asset_loaders/voxelfile_format.h"
#include <string>
#include <memory>
#include "data/voxel_data.h"
#include "asset_loaders/asset_loader.h"

namespace trillek {

class basic_voxel_asset_loader
    : public asset_loader
{
    public:
        basic_voxel_asset_loader();
        virtual ~basic_voxel_asset_loader();
        voxel_data* load(const std::string& file) const override;
        void init() override;
    protected:
    private:
};
}

#endif // BASIC_VOXEL_ASSET_LOADER_H
