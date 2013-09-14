#ifndef CLIENT_H
#define CLIENT_H
#include <memory>
#include "services/event_service.h"
#include "services/graphics_service.h"
#include "services/input_service.h"
#include "services/window_service.h"
#include "services/settings_service.h"
#include "services/asset_service.h"
#include <iostream>
#include <assert.h>
#include <typeindex>
#include <chrono>
#include "general/string_exception.h"

namespace trillek
{

class client
{
    public:
        client();
        virtual ~client();
        void run();
        template <typename T>
        T* get_service()
        {
            if(this->services.find(T::name)!= this->services.end())
            {
                return dynamic_cast<T*>(this->services[T::name].get());
            }

            throw string_exception(
                "Couldn't find "+std::string(T::name)+"-service");
        }
        template <typename T>
        bool has_service()
        {
            return this->services.find(T::name)!= this->services.end();
        }
        template <typename T, typename... A>
        void register_service(A... args)
        {
            if(this->services.find(T::name)!= this->services.end())
            {
                std::cerr <<
                "Warning: Overwriting "+std::string(T::name)+"-service"
                << std::endl;
            }
            this->services[T::name]=make_unique<T>(this,args...);
        }

        template<typename T>
        T* declare_required_service(service* /*from*/=NULL)
        {
            if(!has_service<T>())
            {
                std::cerr << std::string(T::name) <<
                    " was required but not present!" << std::endl;
                requirement_error=true;
                return NULL;
            }else
            {
                T* retval = get_service<T>();
                retval->init();
                return retval;
            }
        }

        enum tick_type
        {
            work,
            pre_render,
            render,
            post_render
        };
        void add_tick_method(tick_type type,std::function<void()> tick_method);
        void exit(std::shared_ptr<event> /*e*/){running=false;}
        static client* current;
        void log(std::string message);
        std::chrono::microseconds current_frame_duration;
    protected:
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> last_frame;
        bool running;
        std::map<std::string,std::unique_ptr<service>> services;
        std::map<tick_type,std::vector<std::function<void()>>> tick_methods;

        bool requirement_error;
        void init_all();
        void init_time();
        void update_time();
        unsigned char max_framerate;
        void limit_framerate();
};

}
#endif // CLIENT_H
