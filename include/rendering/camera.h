#ifndef CAMERA_H
#define CAMERA_H

#include "services/key_codes.h"
#include "rendering/render_tree.h"
#include "services/service.h"

namespace trillek
{

class graphics_service;

class camera : public render_tree_node
{
    public:
        camera();
        virtual ~camera();
        virtual render_data* get_render_data() override {return nullptr;}
        virtual void render(graphics_service* service)=0;
        virtual void mouse_moved(std::shared_ptr<event>)=0;
        virtual void key_pressed(std::shared_ptr<event>)=0;
        virtual void init()=0;
    protected:
    private:
};

}

#endif // CAMERA_H
