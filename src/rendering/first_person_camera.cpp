#include "rendering/first_person_camera.h"
#include "client.h"
#include "services/event_service.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

namespace trillek
{

first_person_camera::first_person_camera()
    :   rot(0,0), pos(0,0,0)
{
    //ctor
}

first_person_camera::~first_person_camera()
{
    //dtor
}

void first_person_camera::init()
{
    using namespace std::placeholders;
    event_service* e_s=client::current->get_service<event_service>();

    e_s->register_for_event(event::event_type::mouse_move,
        std::bind(&first_person_camera::mouse_moved,this,_1));
    e_s->register_for_event(event::event_type::key,
        std::bind(&first_person_camera::key_pressed,this,_1));
}

void first_person_camera::mouse_moved(std::shared_ptr<event> e)
{
    auto mm_e=std::dynamic_pointer_cast<mouse_move_event>(e);

    rot.y+=(mm_e->dx*-1.0f)/10.0f;
    if(rot.y<0)
        rot.y+=360;
    if(rot.y>360)
        rot.y-=360;

    rot.x+=(mm_e->dy*-1.0f)/10.0f;
        if(rot.x<-90)
            rot.x=-90;
        if(rot.x>90)
            rot.x=90;
}

void first_person_camera::key_pressed(std::shared_ptr<event> e)
{
    auto k_e=std::dynamic_pointer_cast<key_event>(e);
    float conv=M_PI/180.0f;
    float _speed = speed*
            client::current->current_frame_duration.count()/1000000;
    if(k_e->code==(keyboard::key_code)'w')
    {
        pos.x+=sin(rot.y*conv)*cos(rot.x*conv)*_speed;
        pos.z+=cos(rot.y*conv)*cos(rot.x*conv)*_speed;
        pos.y-=sin(rot.x*conv)*_speed;
    }
    if(k_e->code==(keyboard::key_code)'s')
    {
        pos.x-=sin(rot.y*conv)*cos(rot.x*conv)*_speed;
        pos.z-=cos(rot.y*conv)*cos(rot.x*conv)*_speed;
        pos.y+=sin(rot.x*conv)*_speed;
    }
    if(k_e->code==(keyboard::key_code)'a')
    {
        pos.x+=cos(rot.y*conv)*_speed;
        pos.z-=sin(rot.y*conv)*_speed;
    }
    if(k_e->code==(keyboard::key_code)'d')
    {
        pos.x-=cos(rot.y*conv)*_speed;
        pos.z+=sin(rot.y*conv)*_speed;
    }
}

void first_person_camera::render(graphics_service* service)
{
    service->do_rotation_x(-rot.x);
    service->do_rotation_y(-rot.y);
    service->do_translation(pos);
}

}
