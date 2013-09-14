#ifndef FIRST_PERSON_CAMERA_H
#define FIRST_PERSON_CAMERA_H

#include "services/key_codes.h"
#include "rendering/camera.h"
#include "math/vector.h"

namespace trillek
{

class first_person_camera : public camera
{
    public:
        first_person_camera();
        virtual ~first_person_camera();
        void render(graphics_service* service);
        void mouse_moved(std::shared_ptr<event> e);
        void key_pressed(std::shared_ptr<event> e);
        void init() override;
        float speed=10; // units per second
    protected:
    private:
        vector2d<float> rot;
        vector3d<float> pos;
};

}

#endif // FIRST_PERSON_CAMERA_H
