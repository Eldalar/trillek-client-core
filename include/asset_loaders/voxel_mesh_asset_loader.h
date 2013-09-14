/*
 * File:   voxel_mesh_asset_loader.h
 * Author: Zcool31
 *
 * Created on September 2, 2013, 12:46 PM
 */

#ifndef VOXEL_MESH_ASSET_LOADER_H
#define	VOXEL_MESH_ASSET_LOADER_H

#include <string>
#include "asset_loaders/asset_loader.h"
#include "data/render_data.h"

namespace trillek {

class voxel_octree;

class voxel_mesh_asset_loader : public asset_loader {
public:
    voxel_mesh_asset_loader();
    virtual ~voxel_mesh_asset_loader();
    data* load(const std::string& file) const override;
    void init();
};

}



#endif	/* VOXEL_MESH_ASSET_LOADER_H */

