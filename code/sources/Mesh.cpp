#include <Mesh.hpp>
#include <Model.hpp>

namespace model_view
{
    Mesh::Mesh(const char * name)
    :
        mesh_name (name)
    {
        
    }

    void Mesh::apply_transform(Model * parent)
    {
        /*static float angle = 0.f;

        angle += 0.025f;
        transform_component.rotation = Vector3f{0.f, 0.f, angle};*/

        mtransformation = Matrix44(1);//m_original_transform;

        mtransformation = translate (mtransformation, transform_component.position);

        mtransformation = rotate_around_x (mtransformation, transform_component.rotation.x);
        mtransformation = rotate_around_y (mtransformation, transform_component.rotation.y);
        mtransformation = rotate_around_z (mtransformation, transform_component.rotation.z);

        mtransformation = scale (mtransformation, transform_component.scale);

        mtransformation = parent->mtransformation * mtransformation;
    }
}