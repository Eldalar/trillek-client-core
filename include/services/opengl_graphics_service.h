#ifndef OPENGL_GRAPHICS_SERVICE_H
#define OPENGL_GRAPHICS_SERVICE_H

#include "services/graphics_service.h"
#include "rendering/camera.h"
namespace trillek
{


struct light_settings;

struct model_recall_data_entry
{
    unsigned int vertices;
    unsigned int size;
};
typedef std::map<material_data*,model_recall_data_entry> model_recall_data;

class opengl_graphics_service
    : public graphics_service
{
    public:
        opengl_graphics_service(client* _client,
            render_algorithm* rendering_algorithm, camera* cam);
        virtual ~opengl_graphics_service();
        void receive_event(std::shared_ptr<event> e);
        void _init();
        void window_resized(float width, float height);
        void drawPolygon(   vector3d<float>& v1,
                            vector3d<float>& v2,
                            vector3d<float>& v3,
                            bool automatic_normal=false);
        void generate_normal(   vector3d<float>& v1,
                                vector3d<float>& v2,
                                vector3d<float>& v3);
        void set_normal(        vector3d<float>& n);
        vector3d<float> calculate_normal(  vector3d<float>& v1,
                                    vector3d<float>& v2,
                                    vector3d<float>& v3);
        void render_light(light_settings options);
        void do_translation(vector3d<float> trans);
        void do_rotation_x(float rot);
        void do_rotation_y(float rot);
        void do_rotation_z(float rot);
        void register_model(uintptr_t ID, std::shared_ptr<mesh_data> model);
        bool is_model_registered(uintptr_t ID);
        void recall_model(uintptr_t ID);
        void bind_material(material_data* material);
    protected:
    private:
        std::map<uintptr_t,model_recall_data> model_recall_buffer;

        unsigned char light_counter;
        void prepare_rendering();
        void rendering();
        void rendering(render_tree_node* node);
        void end_rendering(){};
};

}

#endif // OPENGL_GRAPHICS_SERVICE_H
