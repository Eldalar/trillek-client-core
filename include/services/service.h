#ifndef SERVICE_H
#define SERVICE_H
#include <memory>
#include <vector>
#include <string>
#include "event_service_events.h"

#define SERVICE_NAME(name) constexpr std::string get_name() override {return name;}

namespace trillek
{

class client;

class service
{
    public:
        service(client* _client){this->_client=_client;initialized=false;};
        virtual ~service(){};
        virtual void receive_event(event_shared_ptr) = 0;
        bool is_initialized(){return initialized;};
        void init(){_init();initialized=true;};
        virtual void _init()=0;
        client* get_client(){return _client;}
    protected:
        client* _client;
    private:
        bool initialized;
        //virtual std::vector<std::string> get_requirements()=0;
};

}

#endif // SERVICE_H
