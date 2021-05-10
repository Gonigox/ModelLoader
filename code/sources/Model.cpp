#include <Model.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace example
{
    Model::Model(std::string model_path)
    {
        transform_component.position = Vector3f{ 0.f, 0.f,  -10.f };
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
            auto rootNode = scene->mRootNode;

            aiVector3D  rotation;
            aiVector3D  position;
            aiVector3D  scale;
            //aiMatrix4x4 mTransformation;

            for(unsigned int index = 0; index < rootNode->mNumChildren; ++index)
            {
                auto node = rootNode->mChildren[index];

                if(node->mNumMeshes > 0)
                {
                    shared_ptr< Mesh > mesh = std::make_shared< Mesh >(node->mName.C_Str());

                    size_t number_of_vertices  = 0;
                    size_t number_of_triangles = 0;

                    for(unsigned int index = 0; index < node->mNumMeshes; ++index)
                    { 
                        auto imported_mesh = scene->mMeshes[node->mMeshes[index]];

                        number_of_vertices += imported_mesh->mNumVertices; 

                        mesh->original_vertices.resize(number_of_vertices);

                        for
                        (
                            size_t index = number_of_vertices - imported_mesh->mNumVertices, v_index = 0; 
                            index < number_of_vertices; 
                            ++index , ++v_index
                        )
                        {
                            auto & vertex = imported_mesh->mVertices[v_index];

                            mesh->original_vertices[index] = Point4f(vertex.x, -vertex.y, vertex.z, 1.f);
                        }

                        mesh->transformed_vertices.resize (number_of_vertices);
                            mesh->display_vertices.resize (number_of_vertices);

                        mesh->original_colors.resize (number_of_vertices);

                        for
                        (
                            size_t index = number_of_vertices - imported_mesh->mNumVertices; 
                            index < number_of_vertices; 
                            ++index
                        )
                        {
                            mesh->original_colors[index].set (1.f, 1.f, 1.f);//rand_clamp(), rand_clamp(), rand_clamp());
                        }

                        number_of_triangles += imported_mesh->mNumFaces;

                        mesh->original_indices.resize (number_of_triangles * 3);

                        auto indices_iterator = mesh->original_indices.begin ();
                        indices_iterator     += (number_of_triangles * 3) - (imported_mesh->mNumFaces * 3);

                        int indices_offset = number_of_vertices - imported_mesh->mNumVertices;

                        for (size_t index = 0; index < imported_mesh->mNumFaces; index++)
                        {
                            auto & face = imported_mesh->mFaces[index];

                            assert(face.mNumIndices == 3);              // Una face puede llegar a tener de 1 a 4 índices,
                                                                        // pero nos interesa que solo haya triángulos
                            auto indices = face.mIndices;

                            *indices_iterator++ = int(indices[0]) + indices_offset;
                            *indices_iterator++ = int(indices[1]) + indices_offset;
                            *indices_iterator++ = int(indices[2]) + indices_offset;
                        }

                        /*mTransformation = node->mTransformation;

                        mesh->m_original_transform = Matrix44
                        (
                            mTransformation.a1, mTransformation.a2, mTransformation.a3, mTransformation.a4,
                            mTransformation.b1, mTransformation.b2, mTransformation.b3, mTransformation.b4,
                            mTransformation.c1, mTransformation.c2, mTransformation.c3, mTransformation.c4,
                            mTransformation.d1, mTransformation.d2, mTransformation.d3, mTransformation.d4
                        );*/

                        
                    }

                    node->mTransformation.Decompose(scale, rotation, position);
                    mesh->transform_component.position = Vector3f {position.x, -position.y, position.z};
                    mesh->transform_component.rotation = Vector3f {rotation.x,  rotation.y, rotation.z};
                    mesh->transform_component.scale    = Vector3f {   scale.x,     scale.y,    scale.z};

                    mesh_list.push_back(mesh);
                }
            }
        }
    }

    void Model::update()
    {
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