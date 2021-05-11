
// Este código es de dominio público.
// gonigox@gmail.com
// 2021.04

#pragma once
#include <Mesh.hpp>
#include <memory>

namespace example
{
    using std::shared_ptr;

    /// <summary>
    /// Class container of the meshes charged from a COLLADA or OBJ file.
    /// Also conatins the position of it in the scene and the path of the file
    /// </summary>
    class Model
    {
    public:

        /// <summary>
        /// Transform component of the model
        /// </summary>
        Transform_component          transform_component;
        /// <summary>
        /// Transformation matrix of the model
        /// </summary>
        Matrix44                     mtransformation;

        /// <summary>
        /// Path of the file charged
        /// </summary>
        std::string                  model_file_path;
        /// <summary>
        /// List of all the meshes charged from the file
        /// </summary>
        vector< shared_ptr< Mesh > > mesh_list;

        /// <summary>
        /// Constructor of the model
        /// </summary>
        /// <param name="model_path"> Te path of the file to charge </param>
        Model(std::string model_path);

    public:

        /// <summary>
        /// Updates the position, rotation, scale of the model and all the meshes inside of it
        /// </summary>
        virtual void update();

    private:

        /// <summary>
        /// Function to randomize a number between 0-1.
        /// It is used to randomize the colors of the triangles when charging the model
        /// </summary>
        /// <returns></returns>
        float rand_clamp   () { return float(rand () & 0xff) * 0.0039215f; }
    };
}