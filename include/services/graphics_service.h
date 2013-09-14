#ifndef GRAPHICS_SERVICE_H
#define GRAPHICS_SERVICE_H
#include "services/service.h"
#include "rendering/render_tree.h"
#include "rendering/render_algorithm.h"
#include "rendering/camera.h"
#include "make_unique.h"
#include "data/mesh_data.h"
#include <iostream>
#include <stdint.h>
namespace trillek
{

class graphics_service
    : public service
{
    public:
        graphics_service(client* _client,
                         render_algorithm* rendering_algorithm, camera* cam)
                         : service(_client)
        {
            tree = trillek::make_unique<render_tree>();
         this->rendering_algorithm=
            std::shared_ptr<render_algorithm>(rendering_algorithm);
         this->cam=
            std::shared_ptr<camera>(cam);};
        virtual ~graphics_service(){};

        virtual void render()
        {
            prepare_rendering();
            rendering();
            end_rendering();
        };
        render_tree* get_render_tree(){return tree.get();}
        virtual void drawPolygon(vector3d<float>& v1,
                                 vector3d<float>& v2,
                                 vector3d<float>& v3,
                                 bool automatic_normal)=0;
        virtual void do_translation(vector3d<float> trans)=0;
        virtual void do_rotation_x(float rot)=0;
        virtual void do_rotation_y(float rot)=0;
        virtual void do_rotation_z(float rot)=0;
        virtual void register_model(uintptr_t ID,
                                    std::shared_ptr<mesh_data> model)=0;
        static constexpr char name[]="graphics";
    protected:
        std::shared_ptr<render_algorithm> rendering_algorithm;
        std::shared_ptr<camera> cam;
        std::unique_ptr<render_tree> tree;
    private:
        virtual void prepare_rendering()=0;
        virtual void rendering()=0;
        virtual void end_rendering()=0;
};

}

#endif // GRAPHICS_SERVICE_H
