
// Este código es de dominio público.
// gonigox@gmail.com
// 2021.04

#pragma once
#include <vector>
#include <Color.hpp>
#include <string>
#include <Transform_component.hpp>

namespace example
{
    class Model;
    using std::vector;
    using argb::Rgb888;

    
    /// <summary>
    /// Class mesh containing the data necesary to render it correctly in the scene
    /// </summary>
    class Mesh
    {
    private:
         typedef Rgb888            Color;
         typedef vector< Point4f > Vertex_Buffer;
         typedef vector< int    >  Index_Buffer;
         typedef vector< Color  >  Vertex_Colors;

    public:

        /// <summary>
        /// Bame of the mesh (Determined on the file the model class charge)
        /// </summary>
        std::string         mesh_name;

        /// <summary>
        /// Transfomr component of the mesh relative to the model containing it
        /// </summary>
        Transform_component transform_component;
        /// <summary>
        /// Transformation matrix of the mesh
        /// </summary>
        Matrix44            mtransformation;
        
        /// <summary>
        /// Vector containig the original vertices positions of the mesh
        /// </summary>
        Vertex_Buffer       original_vertices;
        /// <summary>
        /// Vector containig the original indices of the triangles of the mesh
        /// </summary>
        Index_Buffer        original_indices;
        /// <summary>
        /// Vector containig the original colors of the triangles of the mesh
        /// </summary>
        Vertex_Colors       original_colors;
        /// <summary>
        /// Vector to store vertices after the matrix of transformation is apply to them
        /// </summary>
        Vertex_Buffer       transformed_vertices;
        /// <summary>
        /// Vector containig the position of the vertices in display coordinates
        /// </summary>
        vector< Point4i >   display_vertices;

    public:

        /// <summary>
        /// Constructor of a mesh
        /// </summary>
        /// <param name="name"> Name of the mesh </param>
        Mesh(const char * name);

        /// <summary>
        /// Aplly transformations to the mesh relative to its parent model
        /// </summary>
        /// <param name="parent"> Pointer of the parent model </param>
        void apply_transform (Model * parent);
    };
}