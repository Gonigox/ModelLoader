#include <Model.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace example
{
    Model::Model(std::string model_path)
    {
        transform_component.position = Vector3f{ 0.f, 0.5f, -10.f };
        transform_component.rotation = Vector3f{ 0.f, 0.f ,   0.f };
        transform_component.scale    = Vector3f{ 4.f, 4.f ,   4.f };

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
                shared_ptr< Mesh > mesh = std::make_shared< Mesh >();

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
                    mesh->original_colors[index].set (rand_clamp(), rand_clamp(), rand_clamp());
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

    void Model::update()
    {
        // Se actualizan los parámetros de transformatión (sólo se modifica el ángulo):

        static float angle = 0.f;

        angle += 0.025f;
        transform_component.rotation = Vector3f{0.f, angle, 0.f};

        // Se crean las matrices de transformación:
        mtransformation = Matrix44(1);
        mtransformation = translate       (mtransformation, transform_component.position);

        mtransformation = rotate_around_x (mtransformation, transform_component.rotation.x);
        mtransformation = rotate_around_y (mtransformation, transform_component.rotation.y);
        mtransformation = rotate_around_z (mtransformation, transform_component.rotation.z);

        mtransformation = scale           (mtransformation, transform_component.scale);

        
        for(size_t numMesh = 0; numMesh < mesh_list.size(); ++numMesh)
        {
            mesh_list[numMesh].get()->apply_transform(this);
        }
    }
}