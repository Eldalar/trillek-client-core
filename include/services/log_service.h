#ifndef LOG_SERVICE_H
#define LOG_SERVICE_H

#include "services/service.h"

namespace trillek
{

class log_service : public service
{
    public:
        log_service(client* _client);
        virtual ~log_service();
        static constexpr char name[]="log";
    protected:
    private:
};

}

#endif // LOG_SERVICE_H
