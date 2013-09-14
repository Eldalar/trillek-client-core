#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <map>
#include <vector>
#include "services/service.h"
#include "services/event_service_events.h"

namespace trillek
{

class event_service : public service
{
    typedef std::function<void(std::shared_ptr<event>)> event_callback;
    typedef std::multimap<event::event_type,event_callback> event_callbacks_mm;
    public:
        event_service(client* _client);
        virtual ~event_service();

        void send_event(std::shared_ptr<event> e);
        void register_for_event(event::event_type type, service* who);
        void register_for_event(event::event_type type, event_callback who);
        void receive_event(std::shared_ptr<event> /*e*/){}
        void process_events();
        static constexpr char name[] = "event";
        void _init();
    protected:
    private:
        unsigned char current_event_buffer;
        std::vector<std::shared_ptr<event>> event_buffer[2];
        event_callbacks_mm registered_event_callbacks;
};

}

#endif // EVENT_SERVICE_H
