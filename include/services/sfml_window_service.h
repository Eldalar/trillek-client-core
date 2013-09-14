#ifndef SFML_WINDOW_SERVICE_H
#define SFML_WINDOW_SERVICE_H

#include <SFML/Window.hpp>
#include "services/window_service.h"

namespace trillek
{

class event_service;

class sfml_window_service : public window_service
{
    public:
        sfml_window_service(client* _client);
        virtual ~sfml_window_service();
        bool is_open() override;
        void _init() override;
        void close(std::shared_ptr<event> /*e*/);
        void activate() override;
        void finish_frame() override;
        void process() override;
        void set_mouse_pos(float x, float y) override;
        void set_mouse_pos(int x, int y) override;
        vector2d<unsigned int> get_size() override;
        void receive_event(event_shared_ptr) override {}
    protected:
    private:
        event_service* events;
        sf::Window sfml_window;
};

}

#endif // SFML_WINDOW_SERVICE_H
