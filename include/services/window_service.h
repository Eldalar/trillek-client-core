#ifndef WINDOW_SERVICE_H
#define WINDOW_SERVICE_H

#include "services/service.h"
#include "math/vector.h"

namespace trillek
{

class window_service : public service
{
    public:
        window_service(client* _client):service(_client){};
        virtual ~window_service(){};
        //virtual void open()=0;
        //virtual void close()=0;
        virtual bool is_open()=0;
        virtual void activate()=0;
        virtual void finish_frame()=0;
        virtual void process()=0;
        virtual vector2d<unsigned int> get_size()=0;
        virtual void set_mouse_pos(float,float)=0;
        virtual void set_mouse_pos(int,int)=0;
        static constexpr char name[]="window";
    protected:
    private:
};

}

#endif // WINDOW_SERVICE_H
