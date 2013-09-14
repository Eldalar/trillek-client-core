#include "services/event_service.h"
#include "client.h"

namespace trillek
{

event_service::event_service(client* _client)
    :service(_client)
{
    current_event_buffer=0;
    event_buffer[0].reserve(1024);
    event_buffer[1].reserve(1024);
}

event_service::~event_service()
{
    //dtor
}

void event_service::_init()
{
    this->_client->add_tick_method(client::pre_render,
                        std::bind(&event_service::process_events,this));
}

void event_service::send_event(std::shared_ptr<event> e)
{
    event_buffer[current_event_buffer].push_back(e);
}

void event_service::register_for_event( event::event_type type,
                                        service* who)
{
    this->register_for_event(type,std::bind(&service::receive_event,
                             who,
                             std::placeholders::_1));
}

void event_service::register_for_event( event::event_type type,
                                        event_callback who)
{
    this->registered_event_callbacks.insert(
            std::pair<event::event_type,event_callback>(type,who));
}

void event_service::process_events()
{
    // Redirecting incoming events into the other event_buffer
    current_event_buffer=1-current_event_buffer;
    // Handle the events from the old/filled event_buffer
    for (std::shared_ptr<event> e : event_buffer[1-current_event_buffer] )
    {
        std::pair<event_callbacks_mm::iterator,event_callbacks_mm::iterator>
            ret = registered_event_callbacks.equal_range(e->get_type());

        for (   event_callbacks_mm::iterator it=ret.first;
                it!=ret.second; ++it)
        {
            it->second(e);
        }
    }
    event_buffer[1-current_event_buffer].clear();
}

}
