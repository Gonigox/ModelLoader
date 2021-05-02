#include <Model.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace example
{
    Model::Model(std::string model_path)
    {
        model_file_path = model_path;

        Assimp::Importer importer;

        auto scene = importer.ReadFile
        (
            model_file_path,
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
        );


        if(scene)
        {
            for(unsigned int mesh_num = 0; mesh_num < scene->mNumMeshes; ++mesh_num)
            {
                auto imported_mesh = scene->mMeshes[mesh_num];
                shared_ptr< Mesh > mesh (new Mesh());

                size_t number_of_vertices = imported_mesh->mNumVertices;

                mesh->original_vertices.resize(number_of_vertices);

                for(size_t index = 0; index < number_of_vertices; ++index)
                {
                    auto & vertex = imported_mesh->mVertices[index];

                    mesh->original_vertices[index] = Point4f(vertex.x, -vertex.y, vertex.z, 1.f);
                }

                mesh->transformed_vertices.resize (number_of_vertices);
                    mesh->display_vertices.resize (number_of_vertices);

                mesh->original_colors.resize (number_of_vertices);

                for(size_t index = 0; index < number_of_vertices; ++index)
                {
                    mesh->original_colors[index].set (1.f, 1.f, 1.f);
                }

                size_t number_of_triangles = imported_mesh->mNumFaces;

                mesh->original_indices.resize (number_of_triangles * 3);

                auto indices_iterator = mesh->original_indices.begin ();

                for (size_t index = 0; index < number_of_triangles; index++)
                {
                    auto & face = imported_mesh->mFaces[index];

                    assert(face.mNumIndices == 3);              // Una face puede llegar a tener de 1 a 4 índices,
                                                                // pero nos interesa que solo haya triángulos
                    auto indices = face.mIndices;

                    *indices_iterator++ = int(indices[0]);
                    *indices_iterator++ = int(indices[1]);
                    *indices_iterator++ = int(indices[2]);
                }

                mesh_list.push_back(mesh);
            }
        }
    }
}