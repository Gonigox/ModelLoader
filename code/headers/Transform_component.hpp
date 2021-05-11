
// Este código es de dominio público.
// gonigox@gmail.com
// 2021.04

#pragma once
#include <math.hpp>

namespace model_view
{
    /// <summary>
    /// Struct containinf 3 vectors representing the rotation, positiona and scale
    /// Rotation is stored in radians
    /// </summary>
    struct Transform_component
    {
    public:

        Vector3f position;
        Vector3f scale;
        Vector3f rotation;
    };
}
