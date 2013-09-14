#ifndef ASSET_SERVICE_H_H
#define ASSET_SERVICE_H_H

#include "services/service.h"
#include "asset_loaders/asset_loader.h"
#include <map>

namespace trillek
{

class asset_service : public service
{
    public:
        asset_service(client* _client);
        virtual ~asset_service();

        data* load(std::string file);
        void register_asset_loader(const std::string& extension,
                                   asset_loader* new_asset_loader);
        void register_asset_loader(const std::string& extension,
                std::shared_ptr<asset_loader> new_asset_loader);
        void receive_event(event_shared_ptr) override {}
        static constexpr char name[]="asset";
        void _init();
    protected:
    private:
        std::map<std::string,std::shared_ptr<asset_loader> > _asset_loaders;
};

}

#endif // ASSET_SERVICE_H_H
