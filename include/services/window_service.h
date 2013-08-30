#ifndef WINDOW_SERVICE_H
#define WINDOW_SERVICE_H

#include "services\service.h"

namespace trillek
{

class window_service : public service
{
    public:
        window_service(client* _client):service(_client){};
        virtual ~window_service(){};
        virtual void open()=0;
        virtual bool is_open()=0;
        virtual void activate()=0;
        virtual void finish_frame()=0;
        virtual void process()=0;
    protected:
    private:
};

}

#endif // WINDOW_SERVICE_H
