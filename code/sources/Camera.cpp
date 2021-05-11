#include <Camera.hpp>

namespace model_view
{
    Camera::Camera()
    :
        mtransformation(1)
    {
        transform_component.position = Vector3f{ 0.f, 0.f, 0.f };
        transform_component.rotation = Vector3f{ 0.f, 0.f, 0.f };
        transform_component.scale    = Vector3f{ 1.f, 1.f, 1.f };
    }

    Matrix44 Camera::calculate_mtransformation()
    {
        mtransformation = Matrix44(1);//m_original_transform;

        mtransformation = translate (mtransformation, transform_component.position);

        mtransformation = rotate_around_x (mtransformation, transform_component.rotation.x);
        mtransformation = rotate_around_y (mtransformation, transform_component.rotation.y);
        mtransformation = rotate_around_z (mtransformation, transform_component.rotation.z);

        mtransformation = scale (mtransformation, transform_component.scale);

        return mtransformation;
    }

    void Camera::set_position(Vector3f new_position)
    {
        transform_component.position = new_position;
    }

    void Camera::set_rotation(Vector3f new_rotation)
    {
        transform_component.rotation = new_rotation;
    }

    void Camera::move_camera (Vector3f speed)
    {
        transform_component.position += speed;
    }

    //void Camera::rotate_around(Vector3f origin, float angle, float radius)
    //{
    //    
    //    transform_component.position = Vector3f {
    //                                                transform_component.position.x,
    //                                                angle,
    //                                                transform_component.position.z
    //                                            };

    //    mtransformation = glm::lookAt(transform_component.position, origin, Vector3f{ 0.f, 1.f, 0.f });
    //    calculate_mtransformation();

    //    //transform_component.rotation = Vector3f {0.f, angle, 0.f};
    //}
}