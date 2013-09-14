#include "services/asset_service.h"
#include <iostream>
namespace trillek
{

asset_service::asset_service(client* _client)
    : service(_client)
{
    //ctor
}

asset_service::~asset_service()
{
    //dtor
}

data* asset_service::load(std::string file)
{
    std::string extension=file.substr(file.find_first_of('.')+1);
    if(_asset_loaders.find(extension)==_asset_loaders.end())
        std::cerr << "Error: Could not load: " << file << std::endl;
    else
        return _asset_loaders[extension]->load(file);
    return nullptr;
}

void asset_service::register_asset_loader(const std::string& extension,
                                          asset_loader* new_asset_loader)
{
    _asset_loaders[extension] =
            std::unique_ptr<asset_loader>(new_asset_loader);
}

void asset_service::_init()
{
    for(auto& _asset_loader : _asset_loaders)
    {
        _asset_loader.second->init();
    }
}

void asset_service::register_asset_loader(const std::string& extension,
        std::shared_ptr<asset_loader> new_asset_loader) {
    _asset_loaders[extension] = new_asset_loader;
}

}
