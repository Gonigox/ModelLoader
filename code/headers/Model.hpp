
// Este código es de dominio público.
// gonigox@gmail.com
// 2021.04

#pragma once
#include <Mesh.hpp>
#include <memory>

namespace example
{
    using std::shared_ptr;

    class Model
    {
    public:

        Transform_component          transform_component;
        Matrix44                     mtransformation;

        std::string                  model_file_path;
        vector< shared_ptr< Mesh > > mesh_list;

        Model(std::string model_path);

    public:

        virtual void update();

    private:

        float rand_clamp   () { return float(rand () & 0xff) * 0.0039215f; }
    };
}