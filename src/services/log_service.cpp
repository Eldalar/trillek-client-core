#include "services/log_service.h"

namespace trillek
{

log_service::log_service(client* _client)
    : service(_client)
{
    //ctor
}

log_service::~log_service()
{
    //dtor
}

}
