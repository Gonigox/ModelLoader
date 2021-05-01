
// Este código es de dominio público.
// angel.rodriguez@esne.edu
// 2013.12 - 2021.04

#include <cassert>
#include <cmath>
#include <math.hpp>
#include <View.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace example
{

    View::View(unsigned width, unsigned height)
    :
        width       (width ),
        height      (height),
        color_buffer(width, height),
        rasterizer  (color_buffer )
    {
        

        Assimp::Importer importer;

        auto scene = importer.ReadFile
        (
            mesh_file_path,
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
        );

        // Si scene es un puntero nulo significa que el archivo no se pudo cargar con éxito:

        if (scene && scene->mNumMeshes > 0)
        {
            // Para este ejemplo se coge la primera malla solamente:

            auto mesh = scene->mMeshes[0];

            size_t number_of_vertices = mesh->mNumVertices;

            // Se copian los datos de coordenadas de vértices:

            original_vertices.resize (number_of_vertices);

            for (size_t index = 0; index < number_of_vertices; index++)
            {
                auto & vertex = mesh->mVertices[index];

                original_vertices[index] = Vertex(vertex.x, -vertex.y, vertex.z, 1.f);
            }

            transformed_vertices.resize (number_of_vertices);
                display_vertices.resize (number_of_vertices);

            // Se inicializan los datos de color de los vértices con colores aleatorios:

            original_colors.resize (number_of_vertices);

            for (size_t index = 0; index < number_of_vertices; index++)
            {
                original_colors[index].set (rand_clamp (), rand_clamp (), rand_clamp ());
            }

            // Se generan los índices de los triángulos:

            size_t number_of_triangles = mesh->mNumFaces;




            ///////////
            Ligth   = Point3f (1.f, -1.f, -0.35f);
            divisor = 1.f / ( pow(2, (Color::bits / Color::component_count)) - 1 );
            ///////////




            original_indices.resize (number_of_triangles * 3);

            Index_Buffer::iterator indices_iterator = original_indices.begin ();

            for (size_t index = 0; index < number_of_triangles; index++)
            {
                auto & face = mesh->mFaces[index];

                assert(face.mNumIndices == 3);              // Una face puede llegar a tener de 1 a 4 índices,
                                                            // pero nos interesa que solo haya triángulos
                auto indices = face.mIndices;

                *indices_iterator++ = int(indices[0]);
                *indices_iterator++ = int(indices[1]);
                *indices_iterator++ = int(indices[2]);
            }
        }
    }

    void View::update ()
    {
        // Se actualizan los parámetros de transformatión (sólo se modifica el ángulo):

        static float angle = 0.f;

        angle += 0.025f;

        // Se crean las matrices de transformación:

        Matrix44 identity(1);
        Matrix44 scaling     = scale           (identity, 4.f);
        Matrix44 rotation_y  = rotate_around_y (identity, angle);
        Matrix44 translation = translate       (identity, Vector3f{ 0.f, 0.5f, -10.f });
        Matrix44 projection  = perspective     (20, 1, 15, float(width) / height);

        // Creación de la matriz de transformación unificada:

        Matrix44 transformation = projection * translation * rotation_y * scaling;

        // Se transforman todos los vértices usando la matriz de transformación resultante:

        for (size_t index = 0, number_of_vertices = original_vertices.size (); index < number_of_vertices; index++)
        {
            // Se multiplican todos los vértices originales con la matriz de transformación y
            // se guarda el resultado en otro vertex buffer:

            Vertex & vertex = transformed_vertices[index] = transformation * original_vertices[index];

            // La matriz de proyección en perspectiva hace que el último componente del vector
            // transformado no tenga valor 1.0, por lo que hay que normalizarlo dividiendo:

            float divisor = 1.f / vertex.w;

            vertex.x *= divisor;
            vertex.y *= divisor;
            vertex.z *= divisor;
            vertex.w  = 1.f;
        }
    }

    void View::render ()
    {
        // Se convierten las coordenadas transformadas y proyectadas a coordenadas
        // de recorte (-1 a +1) en coordenadas de pantalla con el origen centrado.
        // La coordenada Z se escala a un valor suficientemente grande dentro del
        // rango de int (que es lo que espera fill_convex_polygon_z_buffer).

        Matrix44 identity(1);
        Matrix44 scaling        = scale (identity, float(width/ 2), float(height / 2), 100000000.f);
        Matrix44 translation    = translate (identity, Vector3f{ float(width / 2), float(height / 2), 0.f });
        Matrix44 transformation = translation * scaling;
        

        for (size_t index = 0, number_of_vertices = transformed_vertices.size (); index < number_of_vertices; index++)
        {
            display_vertices[index] = Point4i( transformation * transformed_vertices[index] );            
        }       

        // Se borra el framebúffer y se dibujan los triángulos:

        rasterizer.clear ();

        for (int * indices = original_indices.data (), * end = indices + original_indices.size (); indices < end; indices += 3)
        {
            if (is_frontface (transformed_vertices.data (), indices))
            {
                // Se calcula el color del triangulo iluminado a partir del primer vertice del mismo

                Color triangle_color = calculate_light(transformed_vertices.data(), original_colors.data(), Ligth, indices);
                
                // Se establece el color del polígono

                rasterizer.set_color (triangle_color);

                // Se rellena el polígono:

                rasterizer.fill_convex_polygon_z_buffer (display_vertices.data (), indices, indices + 3);
            }
        }

        // Se copia el framebúffer oculto en el framebúffer de la ventana:

        color_buffer.blit_to_window ();
    }

    bool View::is_frontface (const Vertex * const projected_vertices, const int * const indices)
    {
        const Vertex & v0 = projected_vertices[indices[0]];
        const Vertex & v1 = projected_vertices[indices[1]];
        const Vertex & v2 = projected_vertices[indices[2]];

        // Se asumen coordenadas proyectadas y polígonos definidos en sentido horario.
        // Se comprueba a qué lado de la línea que pasa por v0 y v1 queda el punto v2:

        return ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]) < 0.f);
    }

    Point3f View::calculate_normal(const Vertex & a, const Vertex & b, const Vertex & c)
    {
        Point3f vector1 = b - a;
        Point3f vector2 = c - a;
        
        Point3f normal = glm::cross (vector1, vector2);

        return normal;
    }

    float View::calculate_intensity(const Point3f & normal_vector, const Point3f & ligth_vector)
    {
        float intensity = glm::dot (normal_vector, ligth_vector);
            
        if(intensity > 1)
        {
            intensity = 1;
        }
        else if(intensity < 0)
        {
            intensity = 0;
        }

        return intensity;
    }

    View::Color View::calculate_light(const Vertex * const projected_vertices, const Color * original_color, Point3f ligth_vector, const int * const indices)
    {
        // Se calcula la normal normalizada del triangulo

        Point3f triangle_normal = calculate_normal
            (
                projected_vertices[indices[0]], 
                projected_vertices[indices[1]], 
                projected_vertices[indices[2]]
            );

        triangle_normal = glm::normalize(triangle_normal);

        // Con la normal del triangulo se calcula el valor de la intensidad en él

        float triangle_intesity = calculate_intensity(triangle_normal, ligth_vector);

        // Se modifica el color segun la intensidad de luz recibida

        float r = original_color[indices[0]].red()   * divisor;
        float g = original_color[indices[0]].green() * divisor;
        float b = original_color[indices[0]].blue()  * divisor;

       Color final_color = Color
            ( 
                r * triangle_intesity,  
                g * triangle_intesity, 
                b * triangle_intesity
            );

       return final_color;
    }

}
