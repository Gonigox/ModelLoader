
// Este código es de dominio público.
// gonigox@gmail.com
// 2021.04

#pragma once
#include <Transform_component.hpp>


namespace example
{

    class Camera
    {
    
    public:
        
        Transform_component transform_component;
        Matrix44            mtransformation;

    public:

        Camera ();

        Matrix44 calculate_mtransformation ();

        void set_position (Vector3f new_position);

        void set_rotation (Vector3f new_rotation);

        void set_scale    (Vector3f new_scale);

        void move_camera  (Vector3f speed);

        void rotate_around (Vector3f origin, float angle, float radius);
    
    };
}


