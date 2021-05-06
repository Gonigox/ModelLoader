
// Este código es de dominio público.
// gonigox@gmail.com
// 2021.04

#pragma once
#include <vector>
#include <math.hpp>
#include <Color.hpp>
#include <string>

namespace example
{
    class Model;
    using std::vector;
    using argb::Rgb888;

    struct Transform_component
    {
    public:

        Vector3f position;
        Vector3f scale;
        Vector3f rotation;
    };

    class Mesh
    {
    private:
         typedef Rgb888            Color;
         typedef vector< Point4f > Vertex_Buffer;
         typedef vector< int    >  Index_Buffer;
         typedef vector< Color  >  Vertex_Colors;

    public:

        std::string         mesh_name;

        Transform_component transform_component;
        Matrix44            mtransformation;
        
        Vertex_Buffer       original_vertices;
        Index_Buffer        original_indices;
        Vertex_Colors       original_colors;
        Vertex_Buffer       transformed_vertices;
        vector< Point4i >   display_vertices;

    public:

        Mesh(const char * name);

        void apply_transform (Model * parent);
    };
}