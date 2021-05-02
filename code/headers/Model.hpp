
// Este código es de dominio público.
// gonigox@gmail.com
// 2021.04

#pragma once
#include <Mesh.hpp>
#include <string>
#include <memory>

namespace example
{
    using std::shared_ptr;

    class Model
    {
    public:

        Transform_component   transform_component;

        std::string                  model_file_path;
        vector< shared_ptr< Mesh > > mesh_list;

        Model(std::string model_path);
    };
}