#ifndef EVENT_SERVICE_EVENTS_H
#define EVENT_SERVICE_EVENTS_H

#include "services/service.h"
#include "services/mouse_button_codes.h"
#include "services/key_codes.h"
#include <memory>

namespace trillek
{

struct event
{
    enum event_type
    {
        window_resized,
        key,
        mouse_move,
        mouse_button,
        mouse_wheel,
        exit
    };
    virtual event_type get_type()=0;
};

typedef std::shared_ptr<event> event_shared_ptr;

/*
    This is used for events, that don't need to deliver any
    parameters
*/
struct short_event
    : event
{
    event_type type;
    short_event(event_type _type)
        : type(_type)
    {
    }
    event_type get_type(){return type;}
};

struct window_resized_event
    : event
{
    float width;
    float height;
    window_resized_event(float width, float height)
    {
        this->width=width;
        this->height=height;
    }
    event_type get_type(){return event::window_resized;}
};

struct key_event
    : event
{
    bool pressed,continuation;
    keyboard::key_code code;

    key_event(bool pressed, bool continuation, keyboard::key_code code)
    {
        this->pressed=pressed;
        this->continuation=continuation;
        this->code=code;
    }
    event_type get_type(){return event::key;}
};

struct mouse_move_event
    : event
{
    int dx,dy;

    mouse_move_event(int dx, int dy)
    {
        this->dx=dx;
        this->dy=dy;
    }
    event_type get_type(){return event::mouse_move;}
};

struct mouse_button_event
    : event
{
    bool pressed;
    mouse::mouse_button_code button;

    mouse_button_event(bool pressed,mouse::mouse_button_code button)
    {
        this->pressed = pressed;
        this->button=button;
    }
    event_type get_type(){return event::mouse_button;}
};

struct mouse_wheel_event
    : event
{
    int amount;

    mouse_wheel_event(int amount)
    {
        this->amount = amount;
    }
    event_type get_type(){return event::mouse_wheel;}
};

}

#endif // EVENT_SERVICE_EVENTS_H
