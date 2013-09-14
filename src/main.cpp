#include "client.h"
#include <iostream>
#include <memory>
#include "rendering/voxel_model.h"
#include "rendering/light_source.h"
#include "rendering/transformation_node.h"
#include <iostream>
#include <math.h>
#include "services/asset_service.h"
#include "asset_loaders/basic_voxel_asset_loader.h"
#include "asset_loaders/voxel_mesh_asset_loader.h"
#include "data/voxel_array.h"
#include "rendering/dual_marching_cubes_render_algorithm.h"
#include "rendering/first_person_camera.h"

#include "services/opengl_graphics_service.h"
#include "services/sfml_window_service.h"
#include "services/event_service.h"
#include "services/cust_settings_service.h"
#include "services/input_service.h"
#include "system/voxel_material_system.h"

void load_services(trillek::client* client)
{
    client->register_service<trillek::opengl_graphics_service>(
            new trillek::dual_marching_cubes_render_algorithm(),
            new trillek::first_person_camera());
    client->register_service<trillek::asset_service>();
    client->register_service<trillek::sfml_window_service>();
    client->register_service<trillek::event_service>();
    client->register_service<trillek::cust_settings_service>();
    client->register_service<trillek::input_service>();
}

int main(int argc, char **argv)
{
    trillek::client client;

    load_services(&client);
    // Some test-code to load/create something to test the rendering with
        trillek::graphics_service* g_s = client.get_service<trillek::graphics_service>();
        trillek::render_tree* r_t = g_s->get_render_tree();
        trillek::asset_service* a_s = client.get_service<trillek::asset_service>();
        //basic_voxel_asset_loader* bval=new basic_voxel_asset_loader();
        std::shared_ptr<trillek::voxel_mesh_asset_loader> mesh_loader =
                std::make_shared<trillek::voxel_mesh_asset_loader>();
        a_s->register_asset_loader("voxels",
                new trillek::basic_voxel_asset_loader());
        a_s->register_asset_loader("obj", mesh_loader);
        a_s->register_asset_loader("OBJ", mesh_loader);
        a_s->register_asset_loader("stl", mesh_loader);
        a_s->register_asset_loader("STL", mesh_loader);
        a_s->register_asset_loader("3DS", mesh_loader);
        mesh_loader.reset();

        unsigned int size=256;
        auto v_m=std::make_shared<trillek::voxel_model>(size,size,size);
        std::cerr << "Starting Construction" << std::endl;
        std::string filename;
        if(argc > 1) {
            filename = argv[1];
        } else {
            filename = "test.voxels";
        }
        std::cerr << "Trying to load file " << filename << std::endl;
        trillek::voxel_data* v_d=(trillek::voxel_data*)a_s->load(filename);
        unsigned int size_x=size;
        unsigned int size_y=size/2;
        unsigned int size_z=size;
        if(v_d!=NULL)
        {
            v_m->set_render_data(v_d);
        }else
        {
            std::cerr << "Couldn't load " << filename <<
                    ", creating standard room" << std::endl;
            float offset=1;
            float round_size=20;
            for(std::size_t x=0;x<size_x;x++) {
                for(std::size_t z=0;z<size_z;z++) {
                    for(std::size_t y=0;y<size_y;y++) {
                            if(x>offset-1 && x < size_x-offset &&
                               y>offset-1 && y < size_y-offset &&
                               z>offset-1 && z < size_z-offset) {
                                if( x<=offset||x >=size_x-offset-1||
                                    y<=offset||y >=size_y-offset-1||
                                    z<=offset||z >=size_z-offset-1) {
                                    v_m->set_voxel(x,y,z,trillek::voxel(
                trillek::voxel_material_system::get().get_default_solid()));
                                }
                            }
                    }
                }
            }
        }
        std::cerr << "Finished Construction" << std::endl;
        r_t->get_master()->add_child(v_m);

        auto light = std::make_shared<trillek::light_source>(1,0,0);
        auto trans = std::make_shared<trillek::transformation_node>(size_x/4,
                                                                    size_y/2,
                                                                    size_z/4);

        trans->add_child(light);
        r_t->get_master()->add_child(trans);
    // test-code end

    client.run();
    return 0;
}
