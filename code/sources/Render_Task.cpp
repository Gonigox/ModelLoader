#include <Render_Task.hpp>
#include <Scene.hpp>

namespace model_view
{
    Render_Task::Render_Task(unsigned width, unsigned height, std::string name, Scene * scene)
    :
        width        (width ),
        height       (height),
        color_buffer (width, height),
        rasterizer   (color_buffer )
    {
        this->scene = scene;
        divisor = divisor = 1.f / ( pow(2, (Color::bits / Color::component_count)) - 1 );
    }

    void Render_Task::Render_game()
    {
         rasterizer.clear ();

        // Se convierten las coordenadas transformadas y proyectadas a coordenadas
        // de recorte (-1 a +1) en coordenadas de pantalla con el origen centrado.
        // La coordenada Z se escala a un valor suficientemente grande dentro del
        // rango de int (que es lo que espera fill_convex_polygon_z_buffer).
        
        for(size_t model_num = 0; model_num < scene->model_list.size(); ++model_num)
        {
            shared_ptr< Model > model = scene->model_list[model_num];            

            for(size_t mesh_num = 0; mesh_num < model->mesh_list.size(); ++mesh_num)
            {
                shared_ptr< Mesh > mesh = model->mesh_list[mesh_num];

                // Camera coordinates

                Matrix44 camera_coords = inverse (scene->camera.calculate_mtransformation()) * mesh->mtransformation;

                // Se proyectan las coordenadas de las modelos

                Matrix44 projection = perspective (20, 1, 15, float(width) / height) * camera_coords;

                for 
                (
                    size_t index = 0, number_of_vertices = mesh->original_vertices.size (); 
                    index < number_of_vertices; 
                    index++
                )
                {
                    // Se multiplican todos los vértices originales con la matriz de transformación y
                    // se guarda el resultado en otro vertex buffer:

                    Point4f & vertex = mesh->transformed_vertices[index] = projection * mesh->original_vertices[index];

                    // La matriz de proyección en perspectiva hace que el último componente del vector
                    // transformado no tenga valor 1.0, por lo que hay que normalizarlo dividiendo:

                    float divisor = 1.f / vertex.w;

                    vertex.x *= divisor;
                    vertex.y *= divisor;
                    vertex.z *= divisor;
                    vertex.w  = 1.f;
                }

                // Se convierten las coordenadas transformadas y proyectadas a coordenadas
                // de recorte (-1 a +1) en coordenadas de pantalla con el origen centrado.
                // La coordenada Z se escala a un valor suficientemente grande dentro del
                // rango de int (que es lo que espera fill_convex_polygon_z_buffer).
                Matrix44 identity(1);
                Matrix44 scaling        = scale (identity, float(width/ 2), float(height / 2), 100000000.f);
                Matrix44 translation    = translate (identity, Vector3f{ float(width / 2), float(height / 2), 0.f });
                Matrix44 transformation = translation * scaling;
                

                for 
                (
                    size_t index = 0, number_of_vertices = mesh->transformed_vertices.size (); 
                    index < number_of_vertices; 
                    index++
                )
                {
                    mesh->display_vertices[index] = Point4i( transformation * mesh->transformed_vertices[index] );            
                }

                for 
                (
                    int * indices = mesh->original_indices.data(), * end = indices + mesh->original_indices.size(); 
                    indices < end; 
                    indices += 3
                )
                {
                    if(is_inside_frustrum(mesh->display_vertices.data(), indices))
                    {
                        if (is_frontface (mesh->transformed_vertices.data (), indices))
                        {
                            // Se calcula el color del triangulo iluminado a partir del primer vertice del mismo

                            Color triangle_color = calculate_light
                                                   (
                                                       mesh->transformed_vertices.data(), 
                                                       mesh->original_colors.data(), 
                                                       scene->Ligth, 
                                                       indices
                                                   );
                            
                            // Se establece el color del polígono

                            rasterizer.set_color (triangle_color);

                            // Recorte de triangulos

                            Point4i clipped_vertices[10];
                            int     clipped_indices [10] = {0,1,2,3,4,5,6,7,8,9};

                            int new_vertices_count = clip_polygon
                                                     (
                                                         mesh->display_vertices.data (), 
                                                         indices, 
                                                         indices + 3, 
                                                         clipped_vertices, 
                                                         width, 
                                                         height
                                                     );

                            if(new_vertices_count > 0)
                            { 
                                int * indices = clipped_indices;
                                rasterizer.fill_convex_polygon_z_buffer 
                                           (
                                               clipped_vertices, indices, indices + new_vertices_count
                                           );
                            }
                            // Se rellena el polígono:
                            else
                            {
                                rasterizer.fill_convex_polygon_z_buffer 
                                           (
                                               mesh->display_vertices.data (), indices, indices + 3
                                           );
                            }
                        }
                    }
                }
            }  
        }
        // Se copia el framebúffer oculto en el framebúffer de la ventana:

        color_buffer.blit_to_window ();
    }

    bool Render_Task::is_frontface(const Point4f * const projected_vertices,const int * const indices)
    {
        const Point4f & v0 = projected_vertices[indices[0]];
        const Point4f & v1 = projected_vertices[indices[1]];
        const Point4f & v2 = projected_vertices[indices[2]];

        // Se asumen coordenadas proyectadas y polígonos definidos en sentido horario.
        // Se comprueba a qué lado de la línea que pasa por v0 y v1 queda el punto v2:

        return ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]) < 0.f);
    }

    bool Render_Task::is_inside_frustrum(const Point4i * const display_vertices,const int * const indices)
    {
        bool inside = false;

        for(const int * index = indices, * end = index + 3; index < end; ++index)
        {
            if 
            (
                display_vertices[*index].x > 0      && 
                display_vertices[*index].x < width  && 
                display_vertices[*index].y > 0      && 
                display_vertices[*index].y < height
            )
            {
                inside = true;
            }
        }

        return inside;
    }

    Point3f Render_Task::calculate_normal(const Point4f & a, const Point4f & b, const Point4f & c)
    {
        Point3f vector1 = b - a;
        Point3f vector2 = c - a;
        
        Point3f normal = glm::cross (vector1, vector2);

        return normal;
    }

    float Render_Task::calculate_intensity(const Point3f & normal_vector,const Point3f & ligth_vector)
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

    Render_Task::Color Render_Task::calculate_light
    (
        const Point4f * const projected_vertices,
        const Color   *       original_color,
              Point3f         ligth_vector,
        const int     * const indices
    )
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

    int Render_Task::clip_polygon
    (
              Point4i *       input_vertices,
        const int     * const first,
        const int     * const last,
              Point4i *       clipped_vertices,
              int             viewport_width,
              int             viewport_height
    )
    {
        bool needs_clipping = false;

        for(const int * index = first; index < last; ++index)
        {
            if
            (
                input_vertices[*index].x < 0               || 
                input_vertices[*index].x > viewport_width  || 
                input_vertices[*index].y < 0               || 
                input_vertices[*index].y > viewport_height
            )
            {
                needs_clipping = true;
                break;
            }            
        }

        if (needs_clipping)
        {
            int new_vertices_count = 0;
            
            Point4i aux_clipped_vertices[10];
            int     aux_index [10] = {0,1,2,3,4,5,6,7,8,9};
            
            // Clip up side
            new_vertices_count = clip_polygon_side 
                                 (
                                     input_vertices, first, last, aux_clipped_vertices, 
                                     Point4i{0,0,0,0}, Point4i{viewport_width,0,0,0}
                                 );
            // Clip rigth side
            new_vertices_count = clip_polygon_side 
                                 (
                                     aux_clipped_vertices, aux_index,  aux_index + new_vertices_count, clipped_vertices, 
                                     Point4i{viewport_width,0,0,0}, Point4i{viewport_width,viewport_height,0,0}
                                 );
            // Clip down side
            new_vertices_count = clip_polygon_side 
                                 (
                                     clipped_vertices, aux_index,  aux_index + new_vertices_count, aux_clipped_vertices,
                                     Point4i{viewport_width,viewport_height,0,0}, Point4i{0,viewport_height,0,0}
                                 );
            // Clip left side
            new_vertices_count = clip_polygon_side 
                                 (
                                     aux_clipped_vertices, aux_index,  aux_index + new_vertices_count, clipped_vertices, 
                                     Point4i{0,viewport_height,0,0}, Point4i{0,0,0,0}
                                 );

            return new_vertices_count;
        }
        
        return 0;
    }

    int Render_Task::clip_polygon_side
    (
              Point4i *       input_vertices,
        const int     * const first,
        const int     * const last,
              Point4i *       clipped_vertices,
              Point4i         clip_line_origin,
              Point4i         clip_line_end
    )
    {
        int vertices_count = 0;
        for(const int * index = first, * next = index + 1; index < last; ++index, ++next)
        { 
            if(next >= last)
            {
                next = first;
            }

            int first_point_position = 
                (clip_line_end.x - clip_line_origin.x) * (input_vertices[*index].y - clip_line_origin.y) - 
                (clip_line_end.y - clip_line_origin.y) * (input_vertices[*index].x - clip_line_origin.x);

            int second_point_position = 
                (clip_line_end.x - clip_line_origin.x) * (input_vertices[*next].y - clip_line_origin.y) - 
                (clip_line_end.y - clip_line_origin.y) * (input_vertices[*next].x - clip_line_origin.x);

            // Case 1: When both points are inside
            if (first_point_position > 0 && second_point_position > 0)
            {
                // Only second point is added
                clipped_vertices[vertices_count] = input_vertices[*next];
                ++vertices_count;
            }
            // Case 2: When only first point is outside
            else 
            if (first_point_position <= 0 && second_point_position > 0)
            {
                // Point of intersection with the edge
                // and the second point are added
                clipped_vertices[vertices_count] = lineLineIntersection
                                                   (
                                                       input_vertices[*index], input_vertices[*next], 
                                                       clip_line_origin, clip_line_end
                                                   );
                ++vertices_count;

                clipped_vertices[vertices_count] = input_vertices[*next];
                ++vertices_count;
            }
            // Case 3: When only second point is outside
            else
            if (first_point_position > 0 && second_point_position <= 0)
            {
                // Only point of intersection with the edge is added
                clipped_vertices[vertices_count] = lineLineIntersection
                                                   (
                                                       input_vertices[*index], input_vertices[*next], 
                                                       clip_line_origin, clip_line_end
                                                   );
                ++vertices_count;
            }
            // Case 4: when both points are outside
            else
            {
                // No points are added
            }

        }


        return vertices_count;
    }

    Point4i Render_Task::lineLineIntersection(const Point4i & A, const Point4i & B, const Point3i & C, const Point3i & D)
    {
        // Line AB represented as a1x + b1y = c1
        int a1 = B.y - A.y;
        int b1 = A.x - B.x;
        int c1 = a1 * (A.x) + b1 * (A.y);
      
        // Line CD represented as a2x + b2y = c2
        int a2 = D.y - C.y;
        int b2 = C.x - D.x;
        int c2 = a2 * (C.x) + b2 * (C.y);
      
        int determinant = a1 * b2 - a2 * b1;
      
        if (determinant == 0)
        {
            // The lines are parallel
            return Point4i {0, 0, 0, 1};
        }
        else
        {
            int x = (b2 * c1 - b1 * c2) / determinant;
            int y = (a1 * c2 - a2 * c1) / determinant;

            return Point4i {x, y, 0, 1};
        }
    }
}