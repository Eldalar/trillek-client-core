#ifndef SETTINGS_SERVICE_H
#define SETTINGS_SERVICE_H

#include "services/service.h"
#include <string>
#include <map>

namespace trillek
{

class settings_service : public service
{
    public:
        struct setting
        {
            std::string value;
            std::map<std::string,std::shared_ptr<setting>> childs;

            int to_int(int def);
            float to_float(float def);
            std::string to_string(std::string def);
            std::shared_ptr<setting> get(std::string name);
        };


        settings_service(client* _client);
        virtual ~settings_service(){};
        std::shared_ptr<setting> get(std::string name);
        virtual void receive_event(event_shared_ptr) override {}
        virtual void load(std::string file)=0;
        static constexpr char name[]="settings";
    protected:
        std::shared_ptr<setting> master;
    private:
};

}

#endif // SETTINGS_SERVICE_H
