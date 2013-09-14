#include "services/input_service.h"
#include "client.h"

namespace trillek
{

input_service::input_service(client* _client)
    :service(_client)
{
    this->keys_pressed.resize(keyboard::num_key_codes,false);
    trap_mouse=true;
}

input_service::~input_service()
{
    //dtor
}

void input_service::tick()
{
    for(unsigned int k=0;k<keys_pressed.size();++k)
    {
        if(keys_pressed[k])
        {
            events->send_event(std::make_shared<key_event>
                    (true,true,(keyboard::key_code)k));
        }
    }
}

void input_service::_init()
{
    using namespace std::placeholders;
    this->_client->add_tick_method(client::work,
        std::bind(&input_service::tick,this));

    events=this->_client->declare_required_service<event_service>(this);
    if(events)
    {
        std::function<void(std::shared_ptr<event>)> t=std::bind(&service::receive_event,this,std::placeholders::_1);
        events->register_for_event(event::event_type::key,this);
        events->register_for_event(event::event_type::mouse_move,
            std::bind(&input_service::mouse_move,this,_1));
        events->register_for_event(event::event_type::mouse_button,this);
        events->register_for_event(event::event_type::mouse_wheel,this);
    }
}

void input_service::receive_event(std::shared_ptr<event> e)
{
    auto type=e->get_type();
    if(type==event::key) {
        auto k_e=std::dynamic_pointer_cast<key_event>(e);
        if(!k_e->continuation)
        {
            this->keys_pressed[k_e->code]=k_e->pressed;
            if(k_e->pressed)
                std::cerr << "Key pressed: " << k_e->code << std::endl;
            else
                std::cerr << "Key released: " << k_e->code << std::endl;
            if(k_e->code==keyboard::key_code::escape)
            {
                events->send_event(std::make_shared<short_event>(event::exit));
            }
            else if(k_e->code=='e' && k_e->pressed)
                toggle_trap_mouse();
        }
    }else if(type==event::mouse_move) {
        auto mm_e=std::dynamic_pointer_cast<mouse_move_event>(e);
        this->mouse_move(mm_e);
    }else if(type==event::mouse_button) {
        auto mb_e=std::dynamic_pointer_cast<mouse_button_event>(e);
    }else if(type==event::mouse_wheel) {
        auto mw_e=std::dynamic_pointer_cast<mouse_wheel_event>(e);
    }
}

void input_service::mouse_move(std::shared_ptr<event> e)
{
    auto event=std::dynamic_pointer_cast<mouse_move_event>(e);
}

void input_service::toggle_trap_mouse() {
    trap_mouse = !trap_mouse;
}

}
