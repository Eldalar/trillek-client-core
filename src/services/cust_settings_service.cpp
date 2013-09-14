#include "services/cust_settings_service.h"
#include <fstream>
#include <iostream>
#include <vector>

namespace trillek
{

cust_settings_service::cust_settings_service(client* _client,std::string file)
    : settings_service(_client)
{
    this->load(file);
}

cust_settings_service::~cust_settings_service()
{
    //dtor
}

void cust_settings_service::load(std::string file)
{
    std::ifstream ifile(file,std::ios::binary);
    if(!ifile.is_open())
    {
        std::cerr << "Couldn't open the settings file: " << file << std::endl;
        return;
    }
    std::string line;
    std::shared_ptr<setting> new_master = std::make_shared<setting>();
    std::vector<std::pair<int,std::shared_ptr<setting>>> buffer;
    buffer.push_back(std::pair<int,std::shared_ptr<setting>>(0,new_master));
    std::shared_ptr<setting> last_master_node;
    while(!ifile.eof())
    {
        std::getline(ifile,line);
        line=line.substr(0,line.size()-1);
        std::string key,value;
        if(line.find('=')!=std::string::npos)
        {
            key=line.substr(0,line.find('='));
            value=line.substr(line.find('=')+1,std::string::npos);
        }else
        {
            key=line;
            value="";
        }
        if(key.size()==0||key[0]=='#')
        {
            // Ignore this line
            continue;
        }
        int indentation=0;
        while(key[0]=='\t')
        {
            key=key.substr(1);
            indentation++;
        }
        if(buffer[buffer.size()-1].first < indentation)
        {
            if(last_master_node)
            {
                buffer.push_back(std::pair<int,std::shared_ptr<setting>>(
                                                    indentation,
                                                    last_master_node));
                last_master_node=nullptr;
            }else{
                std::cerr <<
                "Couldn't parse the settings file: " << file
                <<std::endl;
                return;
            }
        }
        while(buffer[buffer.size()-1].first>indentation)
        {
            buffer.pop_back();

        }
        if(value.size()>0) {
            auto s = std::make_shared<setting>();
            s->value=value;
            buffer[buffer.size()-1].second->childs[key]=s;
        }else {
            last_master_node = std::make_shared<setting>();
            buffer[buffer.size()-1].second->childs[key]=last_master_node;
        }
    }

    master=new_master;
}

}
