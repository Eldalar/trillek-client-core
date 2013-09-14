#include "client.h"
#include <unistd.h>
#include <iostream>
#include "make_unique.h"
#include "services/event_service.h"
#include "services/opengl_graphics_service.h"
#include "services/input_service.h"
#include "services/sfml_window_service.h"
#include <SFML/OpenGL.hpp>
#include "services/cust_settings_service.h"
#include "rendering/dual_marching_cubes_render_algorithm.h"
#include "rendering/marching_cubes_render_algorithm.h"
#include "rendering/first_person_camera.h"
#include "services/log_service.h"

namespace trillek
{

client* client::current;

client::client()
{
    current=this;
    this->requirement_error=false;
}

client::~client()
{
    //dtor
}

void client::log(std::string message)
{
    if(this->has_service<log_service>())
    {
    }else
    {
        std::cerr << message << std::endl;
    }
}

void client::init_all()
{
    using namespace std::placeholders;
    for(auto& _service : services)
    {
        if(!_service.second->is_initialized())
            _service.second->init();
    }
    if(!this->has_service<event_service>()){
        this->log("Error: Missing event-service");
        requirement_error=true;
    }else
    {
        event_service* e_s=this->get_service<event_service>();
        e_s->register_for_event(event::exit,
            std::bind(&client::exit,this,_1));
    }
    if(!this->has_service<settings_service>()){
        this->log("Error: Missing settings-service");
        requirement_error=true;
    }else{
        settings_service* s_s=this->get_service<settings_service>();
        max_framerate = s_s->get("max_framerate")->to_int(60);
    }
    if(!this->has_service<log_service>()){
        this->log("Warning: Missing log-service");
    }
    init_time();
}

void client::add_tick_method(tick_type type,std::function<void()> tick_method)
{
    this->tick_methods[type].push_back(tick_method);
}

void client::init_time()
{
    last_frame = std::chrono::high_resolution_clock::now();
}

void client::update_time()
{
    auto now=std::chrono::high_resolution_clock::now();
    current_frame_duration=
        std::chrono::duration_cast<std::chrono::microseconds>(now-last_frame);
    last_frame=now;
}

void client::limit_framerate()
{
    auto now=std::chrono::high_resolution_clock::now();
    auto in_use=
        std::chrono::duration_cast<std::chrono::microseconds>(now-last_frame);
    usleep((1.0f/max_framerate)*1000000.0f-in_use.count());
}

void client::run()
{
    init_all();
    if(requirement_error) {
        return;
    }

    while(running)
    {
        update_time();
        for(unsigned int i=0; i<tick_methods[work].size();++i)
            tick_methods[work][i]();
        for(unsigned int i=0; i<tick_methods[pre_render].size();++i)
            tick_methods[pre_render][i]();
        for(unsigned int i=0; i<tick_methods[render].size();++i)
            tick_methods[render][i]();
        limit_framerate();
        for(unsigned int i=0; i<tick_methods[post_render].size();++i)
            tick_methods[post_render][i]();
    }
    current=NULL;
}

}
