#ifndef INPUT_SERVICE_H
#define INPUT_SERVICE_H

#include "services/service.h"
#include "services/key_codes.h"
#include <vector>

namespace trillek
{

class event_service;

class input_service : public service
{
    std::vector<bool> keys_pressed;
    public:
        input_service(client* _client);
        virtual ~input_service();
        void _init() override;
        void tick();
        void receive_event(std::shared_ptr<event> e);
        bool is_key_pressed(keyboard::key_code code){return keys_pressed[code];}
        static constexpr char name[]="input";
    protected:
    private:
        event_service* events;
        bool trap_mouse;
        void mouse_move(std::shared_ptr<event> e);
        void toggle_trap_mouse();
};

}

#endif // INPUT_SERVICE_H
