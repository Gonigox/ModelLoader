#include <Mesh.hpp>
#include <Model.hpp>

namespace example
{
    Mesh::Mesh()
    {
        transform_component.position = Vector3f{ 0.f, 0.f, 0.f };
        transform_component.rotation = Vector3f{ 0.f, 0.f, 0.f };
        transform_component.scale    = Vector3f{ 1.f, 1.f, 1.f };
    }

    void Mesh::apply_transform(Model * parent)
    {
        mtransformation = Matrix44 (1);

        mtransformation = translate (mtransformation, transform_component.position);

        mtransformation = rotate_around_x (mtransformation, transform_component.rotation.x);
        mtransformation = rotate_around_y (mtransformation, transform_component.rotation.y);
        mtransformation = rotate_around_z (mtransformation, transform_component.rotation.z);

        mtransformation = scale (mtransformation, transform_component.scale);

        mtransformation = parent->mtransformation * mtransformation;
    }
}