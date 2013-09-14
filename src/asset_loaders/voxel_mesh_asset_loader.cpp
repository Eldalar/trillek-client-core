#include "asset_loaders/voxel_mesh_asset_loader.h"
#include "data/mesh_data.h"
#include "data/voxel_octree.h"
#include "data/voxel_array.h"
#include "data/basic_octree.h"
#include "client.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include <array>
#include <cassert>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <functional>
#include <algorithm>
#include <random>
#include "system/voxel_material_system.h"

namespace trillek {

static std::default_random_engine trillek_rand;

typedef vector2d<float> float_vector2d;
typedef vector3d<float> float_vector3d;
typedef vector2d<std::size_t> size_vector2d;
typedef vector2d<signed long int> int_vector2d;
typedef vector3d<signed long int> int_vector3d;
typedef std::array<float_vector3d, 4> float_triangle3d;
typedef std::array<float_vector2d, 3> float_triangle2d;
typedef std::vector<float_triangle3d> triangle3d_vector;
typedef std::map<int_vector2d, std::vector<
        std::reference_wrapper<const float_triangle3d> > >
        bucket_map;
voxel_octree voxelize_mesh(const triangle3d_vector& all_triangles);
/**
 * @brief Test if z ray at xy intersects triangle
 * @param triangle
 * @param xy
 * @param out Z position of intersection if any
 * @return true if intersected, false otherwise
 */
bool z_ray_intersection(const float_triangle3d& triangle,
        const float_vector2d& xy, float& out);
bool y_ray_intersection(const float_triangle3d& triangle,
        const float_vector2d& zx, float& out);
bool x_ray_intersection(const float_triangle3d& triangle,
        const float_vector2d& yz, float& out);
int x_compute_winding(const float_triangle2d& triangle,
        const float_vector2d& xy);
int y_compute_winding(const float_triangle2d& triangle,
        const float_vector2d& xy);
bool consistent_point_in_triangle(const float_triangle2d& triangle,
        const float_vector2d& xy);

voxel_mesh_asset_loader::voxel_mesh_asset_loader() {}
voxel_mesh_asset_loader::~voxel_mesh_asset_loader() {}
data* voxel_mesh_asset_loader::load(const std::string& file) const {
    Assimp::Importer assimporter;
    const aiScene* scene =
            assimporter.ReadFile(file,
                                 aiProcess_JoinIdenticalVertices |
                                 aiProcess_OptimizeGraph |
                                 aiProcess_OptimizeMeshes |
                                 aiProcess_RemoveComponent |
                                 aiProcess_Triangulate |
                                 aiProcess_PreTransformVertices);
    if(!scene) {
        throw std::runtime_error(assimporter.GetErrorString());
    }
    triangle3d_vector all_triangles;
    std::cerr << "File has " << scene->mNumMeshes << " meshes" << std::endl;
    //we ignore most things and process the meshes only
    for(std::size_t i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh& cur_mesh = *(scene->mMeshes[i]);
        std::cerr << "Mesh " << i << " has " << cur_mesh.mNumFaces <<
                " faces" << std::endl;
        for(std::size_t face_i = 0; face_i < cur_mesh.mNumFaces; ++face_i) {
            const aiFace& face = cur_mesh.mFaces[face_i];
            float_triangle3d cur_face;
            for(std::size_t vertex_i = 0; vertex_i < face.mNumIndices;
                    ++vertex_i) {
                assert(vertex_i < 3);
                const aiVector3D& vertex =
                        cur_mesh.mVertices[face.mIndices[vertex_i]];
                cur_face[vertex_i] = make_vector3d(vertex.x,
                                                   vertex.y,
                                                   vertex.z);
            }
            const float_vector3d normal_vector =
                    (cur_face[1] - cur_face[0]).cross(
                    cur_face[2] - cur_face[0]).normalize();
            cur_face[3] = normal_vector;
            all_triangles.emplace_back(std::move(cur_face));
        }
    }
    std::cerr << "Extracted " << all_triangles.size()
            << " triangles" << std::endl;
    return new voxel_octree(voxelize_mesh(all_triangles));
}

bool z_ray_intersection(const float_triangle3d& triangle,
        const float_vector2d& xy, float& out) {
    const float_triangle2d triangle2d = {{
            float_vector2d(triangle[0].x, triangle[0].y),
            float_vector2d(triangle[1].x, triangle[1].y),
            float_vector2d(triangle[2].x, triangle[2].y)}};
    if(consistent_point_in_triangle(triangle2d, xy)) {
        const float_vector3d& norm = triangle[3];
        float_vector2d point = xy - triangle2d[0];
        out = triangle[0].z - point.x * (norm.x / norm.z) -
                point.y * (norm.y / norm.z);
        return true;
    } else {
        return false;
    }
}
bool y_ray_intersection(const float_triangle3d& triangle,
        const float_vector2d& zx, float& out) {
    const float_triangle2d triangle2d = {{
            float_vector2d(triangle[0].z, triangle[0].x),
            float_vector2d(triangle[1].z, triangle[1].x),
            float_vector2d(triangle[2].z, triangle[2].x)}};
    if(consistent_point_in_triangle(triangle2d, zx)) {
        const float_vector3d& norm = triangle[3];
        float_vector2d point = zx - triangle2d[0];
        out = triangle[0].y - point.x * (norm.z / norm.y) -
                point.y * (norm.x / norm.y);
        return true;
    } else {
        return false;
    }
}
bool x_ray_intersection(const float_triangle3d& triangle,
        const float_vector2d& yz, float& out) {
    const float_triangle2d triangle2d = {{
            float_vector2d(triangle[0].y, triangle[0].z),
            float_vector2d(triangle[1].y, triangle[1].z),
            float_vector2d(triangle[2].y, triangle[2].z)}};
    if(consistent_point_in_triangle(triangle2d, yz)) {
        const float_vector3d& norm = triangle[3];
        float_vector2d point = yz - triangle2d[0];
        out = triangle[0].x - point.x * (norm.y / norm.x) -
                point.y * (norm.z / norm.x);
        return true;
    } else {
        return false;
    }
}

int x_compute_winding(const float_triangle2d& triangle,
        const float_vector2d& xy) {
    int ret = 0;
    for(std::size_t i = 0; i < 3; ++i) {
        std::size_t j = (i + 1) % 3;
        const float_vector2d a = triangle[i];
        const float_vector2d b = triangle[j];
        if(a.y <= xy.y) {
            if(b.y > xy.y) {
                if((b - a).cross(xy - a) > 0)
                    ++ret;
            }
        } else if(b.y <= xy.y) {
            if((b - a).cross(xy - a) < 0)
                --ret;
        }
    }
    return ret;
}
int y_compute_winding(const float_triangle2d& triangle,
        const float_vector2d& xy) {
    //swap xy and invoke x_compute_winding
    auto swapxy = [](const float_vector2d& arg)->float_vector2d {
        return make_vector2d(arg.y, arg.x);
    };
    const float_triangle2d triangle_yx = {{
            swapxy(triangle[0]),
            swapxy(triangle[1]),
            swapxy(triangle[2])}};
    const float_vector2d yx = swapxy(xy);
    return x_compute_winding(triangle_yx, yx);
}
bool consistent_point_in_triangle(const float_triangle2d& triangle,
        const float_vector2d& xy) {
    std::normal_distribution<> norm(0.0, 0.01);
    float_vector2d cxy = xy;
    bool x = false;
    bool y = false;
    do {
        x = x_compute_winding(triangle, cxy) != 0;
        y = y_compute_winding(triangle, cxy) != 0;
        //monte carlo
        cxy += make_vector2d(norm(trillek_rand), norm(trillek_rand));
    } while(x != y);
    return x && y;
}
voxel_octree voxelize_mesh(const triangle3d_vector& all_triangles) {
    //float or double or similar
    static const auto MIN_VAL = std::numeric_limits<
            triangle3d_vector::value_type::value_type::value_type>::min();
    //float or double or similar
    static const auto MAX_VAL = std::numeric_limits<
            triangle3d_vector::value_type::value_type::value_type>::max();
    //signed integral type
    static const auto MIN_INT_VAL = std::numeric_limits<
            int_vector3d::value_type>::min();
    static const auto MAX_INT_VAL = std::numeric_limits<
            int_vector3d::value_type>::max();
    typedef std::vector<float> crossing_vector;
    //bucket sort triangles along each plane
    bucket_map xyb, yzb, zxb;
    //integral extents
    int_vector3d min_xyz(MAX_INT_VAL, MAX_INT_VAL, MAX_INT_VAL);
    int_vector3d max_xyz(MIN_INT_VAL, MIN_INT_VAL, MIN_INT_VAL);
    auto triangle_extents = [](const float_triangle3d& arg)
            ->std::pair<float_vector3d, float_vector3d> {
        float_vector3d min_extents(MAX_VAL, MAX_VAL, MAX_VAL);
        float_vector3d max_extents(MIN_VAL, MIN_VAL, MIN_VAL);
        for(std::size_t i = 0; i < 3; ++i) {
            const float_vector3d& cur_vertex = arg[i];
            min_extents.x = std::min(min_extents.x, cur_vertex.x);
            min_extents.y = std::min(min_extents.y, cur_vertex.y);
            min_extents.z = std::min(min_extents.z, cur_vertex.z);
            max_extents.x = std::max(max_extents.x, cur_vertex.x);
            max_extents.y = std::max(max_extents.y, cur_vertex.y);
            max_extents.z = std::max(max_extents.z, cur_vertex.z);
        }
        return std::make_pair(min_extents, max_extents);
    };
    auto bucket_triangle = [&xyb, &yzb, &zxb, &min_xyz, &max_xyz](
            const float_vector3d& min_extents,
            const float_vector3d& max_extents,
            const float_triangle3d& arg)->std::size_t {
        std::size_t ret = 0;
        int_vector3d min_int(
                std::floor(min_extents.x),
                std::floor(min_extents.y),
                std::floor(min_extents.z));
        int_vector3d max_int(
                std::ceil(max_extents.x),
                std::ceil(max_extents.y),
                std::ceil(max_extents.z));
        max_int += int_vector3d(1,1,1);
        //update overall extents
        min_xyz.x = std::min(min_xyz.x, min_int.x);
        min_xyz.y = std::min(min_xyz.y, min_int.y);
        min_xyz.z = std::min(min_xyz.z, min_int.z);
        max_xyz.x = std::max(max_xyz.x, max_int.x);
        max_xyz.y = std::max(max_xyz.y, max_int.y);
        max_xyz.z = std::max(max_xyz.z, max_int.z);
        //old way - stored duplicates of many triangles
//        for(int_vector3d::value_type z = min_int.z;
//                z != max_int.z; ++z) {
//            for(int_vector3d::value_type y = min_int.y;
//                    y != max_int.y; ++y) {
//                for(int_vector3d::value_type x = min_int.x;
//                        x != max_int.x; ++x) {
//                    xyb[int_vector2d(x,y)].push_back(std::ref(arg));
//                    yzb[int_vector2d(y,z)].push_back(std::ref(arg));
//                    zxb[int_vector2d(z,x)].push_back(std::ref(arg));
//                    ++ret;
//                }
//            }
//        }
        //old way filled the volume
        //new way fills the surface of the cube
        for(int_vector3d::value_type x = min_int.x;
                x != max_int.x; ++x) {
            for(int_vector3d::value_type y = min_int.y;
                    y != max_int.y; ++y) {
                xyb[int_vector2d(x,y)].push_back(std::ref(arg));
            }
        }
        for(int_vector3d::value_type y = min_int.y;
                y != max_int.y; ++y) {
            for(int_vector3d::value_type z = min_int.z;
                    z != max_int.z; ++z) {
                yzb[int_vector2d(y, z)].push_back(std::ref(arg));
            }
        }
        for(int_vector3d::value_type z = min_int.z;
                z != max_int.z; ++z) {
            for(int_vector3d::value_type x = min_int.x;
                    x != max_int.x; ++x) {
                zxb[int_vector2d(z, x)].push_back(std::ref(arg));
            }
        }
        return ret;
    };
    std::cerr << "Building buckets" << std::endl;
    for(const float_triangle3d& triangle : all_triangles) {
        float_vector3d min_extents, max_extents;
        std::tie(min_extents, max_extents) = triangle_extents(triangle);
        //std::cerr <<
                bucket_triangle(min_extents, max_extents, triangle);
        //std::cerr << ", ";
    }
    std::cerr << "Bucket counts are " <<
            "\n\t" << xyb.size() <<
            "\n\t" << yzb.size() <<
            "\n\t" << zxb.size() << std::endl;
    voxel_data::size_vector3d size_extent = (max_xyz - min_xyz);
    voxel_array_alternate xyvox, yzvox, zxvox;
    xyvox.reserve_space(size_extent);
    yzvox.reserve_space(size_extent);
    zxvox.reserve_space(size_extent);
    static const float_vector2d xy_center(0.5, 0.5);
    std::cerr << "Voxelizing x-y plane... ";
    for(const bucket_map::value_type& xyvt : xyb) {
        crossing_vector crossings;
        for(const float_triangle3d& triangle : xyvt.second) {
            float_vector3d::value_type crossing;
            if(z_ray_intersection(triangle, xyvt.first + xy_center,
                    crossing)) {
                crossings.push_back(crossing);
            }
        }
        std::sort(crossings.begin(), crossings.end());
        for(std::size_t i = 0; i + 1 < crossings.size(); ++i) {
            for(float coord = crossings[i]; coord < crossings[i + 1];
                    ++coord) {
                int_vector3d::value_type coord_i =
                        static_cast<int_vector3d::value_type>(coord + 0.5);
                xyvox.set_voxel(
                        xyvt.first.x - min_xyz.x,
                        xyvt.first.y - min_xyz.y,
                        coord_i - min_xyz.z,
                        voxel_material_system::get().get_default_solid());
            }
        }
    }
    std::cerr << "DONE!" << std::endl;
    std::cerr << "Voxelizing y-z plane... ";
    for(const bucket_map::value_type& yzvt : yzb) {
        crossing_vector crossings;
        for(const float_triangle3d& triangle : yzvt.second) {
            float_vector3d::value_type crossing;
            if(x_ray_intersection(triangle, yzvt.first + xy_center,
                    crossing)) {
                crossings.push_back(crossing);
            }
        }
        std::sort(crossings.begin(), crossings.end());
        for(std::size_t i = 0; i + 1 < crossings.size(); ++i) {
            for(float coord = crossings[i]; coord < crossings[i + 1];
                    ++coord) {
                int_vector3d::value_type coord_i =
                        static_cast<int_vector3d::value_type>(coord + 0.5);
                yzvox.set_voxel(
                        coord_i - min_xyz.x,
                        yzvt.first.x - min_xyz.y,
                        yzvt.first.y - min_xyz.z,
                        voxel_material_system::get().get_default_solid());
            }
        }
    }
    std::cerr << "DONE!" << std::endl;
    std::cerr << "Voxelizing z-x plane... ";
    for(const bucket_map::value_type& zxvt : zxb) {
        crossing_vector crossings;
        for(const float_triangle3d& triangle : zxvt.second) {
            float_vector3d::value_type crossing;
            if(y_ray_intersection(triangle, zxvt.first + xy_center,
                    crossing)) {
                crossings.push_back(crossing);
            }
        }
        std::sort(crossings.begin(), crossings.end());
        for(std::size_t i = 0; i + 1 < crossings.size(); ++i) {
            for(float coord = crossings[i]; coord < crossings[i + 1];
                    ++coord) {
                int_vector3d::value_type coord_i =
                        static_cast<int_vector3d::value_type>(coord + 0.5);
                zxvox.set_voxel(
                        zxvt.first.y - min_xyz.x,
                        coord_i - min_xyz.y,
                        zxvt.first.x - min_xyz.z,
                        voxel_material_system::get().get_default_solid());
            }
        }
    }
    std::cerr << "DONE!" << std::endl;
    std::size_t xy_count = 0;
    std::size_t yz_count = 0;
    std::size_t zx_count = 0;
    voxel_octree ret;
    ret.reserve_space(size_extent);
    for(std::size_t z = 0; z < size_extent.z; ++z) {
        for(std::size_t y = 0; y < size_extent.y; ++y) {
            for(std::size_t x = 0; x < size_extent.x; ++x) {
                int count = 0;
                if(xyvox.get_voxel(x,y,z).is_solid()) {
                    ++count;
                    ++xy_count;
                }
                if(yzvox.get_voxel(x,y,z).is_solid()) {
                    ++count;
                    ++yz_count;
                }
                if(zxvox.get_voxel(x,y,z).is_solid()) {
                    ++count;
                    ++zx_count;
                }
                if(count == 3) {
                    ret.set_voxel(x, y, z,
                        voxel_material_system::get().get_default_solid());
                }
            }
        }
    }
    std::cerr << "Voxel counts are: " <<
            "\n\t" << xy_count <<
            "\n\t" << yz_count <<
            "\n\t" << zx_count << std::endl;
    std::cerr << "Final result has filled volume " << ret.get_opaque_volume()
            << std::endl;
    std::cerr << "Final result has nodes " << ret.get_num_nodes() << std::endl;
    return ret;
}

void voxel_mesh_asset_loader::init()
{
}

}
