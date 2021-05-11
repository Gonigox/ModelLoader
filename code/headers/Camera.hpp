
// Este código es de dominio público.
// gonigox@gmail.com
// 2021.04

#pragma once
#include <Transform_component.hpp>


namespace example
{

    /// <summary>
    /// Class camera.
    /// Constains its transform component
    /// </summary>
    class Camera
    {
    
    public:
        
        /// <summary>
        /// Transform component of the camera
        /// </summary>
        Transform_component transform_component;
        /// <summary>
        /// Transformation matrix of the camera
        /// </summary>
        Matrix44            mtransformation;

    public:

        /// <summary>
        /// Default contructor of the camera
        /// </summary>
        Camera ();

        /// <summary>
        /// Calculate the transformation matrix with transform component data
        /// </summary>
        /// <returns></returns>
        Matrix44 calculate_mtransformation ();

        /// <summary>
        /// Cahnge position of the camera
        /// </summary>
        /// <param name="new_position"> New position of the camera </param>
        void set_position (Vector3f new_position);

        /// <summary>
        /// Change rotation of the camera
        /// </summary>
        /// <param name="new_rotation"></param>
        void set_rotation (Vector3f new_rotation);

        /// <summary>
        /// Move the camera one time per frame
        /// </summary>
        /// <param name="speed"> Speed of the camera in each of the axis </param>
        void move_camera  (Vector3f speed);

        //void rotate_around (Vector3f origin, float angle, float radius);
    
    };
}


