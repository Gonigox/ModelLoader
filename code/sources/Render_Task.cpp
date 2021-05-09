#include <Render_Task.hpp>
#include <Scene.hpp>

namespace example
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

                            // Recorte de triangulos? We'll see
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
                                rasterizer.fill_convex_polygon_z_buffer (clipped_vertices, indices, indices + new_vertices_count);
                            }
                            // Se rellena el polígono:
                            else
                            {
                                rasterizer.fill_convex_polygon_z_buffer (mesh->display_vertices.data (), indices, indices + 3);
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
            
            new_vertices_count = clip_polygon_side (input_vertices, first, last, clipped_vertices, Point4i{0,0,0,0}, Point4i{viewport_width,0,0,0}, new_vertices_count);
            std::copy(clipped_vertices, clipped_vertices + new_vertices_count, aux_clipped_vertices);
            new_vertices_count = clip_polygon_side (clipped_vertices, aux_index,  aux_index + new_vertices_count, aux_clipped_vertices, Point4i{viewport_width,0,0,0}, Point4i{viewport_width,viewport_height,0,0}, new_vertices_count);
            std::copy(clipped_vertices, clipped_vertices + new_vertices_count, aux_clipped_vertices);
            new_vertices_count = clip_polygon_side (clipped_vertices, aux_index,  aux_index + new_vertices_count, aux_clipped_vertices, Point4i{0,viewport_height,0,0}, Point4i{viewport_width,viewport_height,0,0}, new_vertices_count);
            std::copy(clipped_vertices, clipped_vertices + new_vertices_count, aux_clipped_vertices);
            new_vertices_count = clip_polygon_side (clipped_vertices, aux_index,  aux_index + new_vertices_count, aux_clipped_vertices, Point4i{0,0,0,0}, Point4i{0,viewport_height,0,0}, new_vertices_count);

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
              Point4i         clip_line_end,
              int             vertices_count
    )
    {

        for(const int * index = first, * next = index + 1; index < last; ++index, ++next)
        { 
            Point4i intersection_point;

            if(next >= last)
            {
                next = first;
            }

            if
            ( 
                doIntersect (input_vertices[*index], input_vertices[*next], clip_line_origin, clip_line_end )
            )
            {
                if
                (                
                    lineLineIntersection
                    (
                        input_vertices[*index], 
                        input_vertices[*next], 
                        clip_line_origin, 
                        clip_line_end, 
                        intersection_point
                    )
                )
                {
                    clipped_vertices[vertices_count] = intersection_point;
                    ++vertices_count;

                    if 
                    (
                        input_vertices[*next].x > 0      && 
                        input_vertices[*next].x < width  && 
                        input_vertices[*next].y > 0      && 
                        input_vertices[*next].y < height
                    )
                    {
                        auto a = input_vertices[*next];
                        clipped_vertices[vertices_count] = input_vertices[*next];
                        ++vertices_count;
                    }
                }

            }
            else 
            {
                if 
                (
                    input_vertices[*index].x > 0      && 
                    input_vertices[*index].x < width  && 
                    input_vertices[*index].y > 0      && 
                    input_vertices[*index].y < height
                )
                { 
                    bool save = true;
                    for(const int * i = first, * end = i + vertices_count; i < end; ++i)
                    {
                        if(input_vertices[*index] == clipped_vertices[*i])
                        {
                            save = false;
                        }
                    }
                    if(save)
                    {
                        auto a = input_vertices[*index];
                        clipped_vertices[vertices_count] = input_vertices[*index];
                        ++vertices_count; 
                    }
                    
                }                

                if 
                (
                    input_vertices[*next].x > 0      && 
                    input_vertices[*next].x < width  && 
                    input_vertices[*next].y > 0      && 
                    input_vertices[*next].y < height
                )
                {
                    bool save = true;
                    for(const int * i = first, * end = i + vertices_count; i < end; ++i)
                    {
                        if(input_vertices[*next] == clipped_vertices[*i])
                        {
                            save = false;
                        }
                    }
                    if(save)
                    {
                        auto a = input_vertices[*next];
                        clipped_vertices[vertices_count] = input_vertices[*next];
                        ++vertices_count; 
                    }
                }                    
            }
            
        }

        return vertices_count;
    }

    bool Render_Task::lineLineIntersection(Point4i A, Point4i B, Point3i C, Point3i D, Point4i & intersection)
    {
        // Line AB represented as a1x + b1y = c1
        double a1 = B.y - A.y;
        double b1 = A.x - B.x;
        double c1 = a1 * (A.x) + b1 * (A.y);
      
        // Line CD represented as a2x + b2y = c2
        double a2 = D.y - C.y;
        double b2 = C.x - D.x;
        double c2 = a2 * (C.x) + b2 * (C.y);
      
        double determinant = a1 * b2 - a2 * b1;
      
        if (determinant == 0)
        {
            // The lines are parallel. This is simplified
            // by returning a pair of FLT_MAX
            return false;
        }
        else
        {
            double x = (b2 * c1 - b1 * c2) / determinant;
            double y = (a1 * c2 - a2 * c1) / determinant;

            intersection = Point4i {x, y, 0, 1};

            return true;
        }
    }

    bool Render_Task::onSegment(Point4i p,Point4i q,Point4i r)
    {
        if 
        (
            q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
            q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y)
        )
        {
            return true;
        }
  
        return false;
    }

    int Render_Task::orientation(Point4i p,Point4i q,Point4i r)
    {
        // See https://www.geeksforgeeks.org/orientation-3-ordered-points/
        // for details of below formula.
        int val = (q.y - p.y) * (r.x - q.x) -
                  (q.x - p.x) * (r.y - q.y);
  
        if (val == 0) return 0;  // colinear
  
        return (val > 0)? 1: 2; // clock or counterclock wise
    }

    bool Render_Task::doIntersect(Point4i p1,Point4i q1,Point4i p2,Point4i q2)
    {
        // Find the four orientations needed for general and
        // special cases
        int o1 = orientation(p1, q1, p2);
        int o2 = orientation(p1, q1, q2);
        int o3 = orientation(p2, q2, p1);
        int o4 = orientation(p2, q2, q1);
  
        // General case
        if (o1 != o2 && o3 != o4)
            return true;
  
        // Special Cases
        // p1, q1 and p2 are colinear and p2 lies on segment p1q1
        if (o1 == 0 && onSegment(p1, p2, q1)) return true;
  
        // p1, q1 and q2 are colinear and q2 lies on segment p1q1
        if (o2 == 0 && onSegment(p1, q2, q1)) return true;
  
        // p2, q2 and p1 are colinear and p1 lies on segment p2q2
        if (o3 == 0 && onSegment(p2, p1, q2)) return true;
  
         // p2, q2 and q1 are colinear and q1 lies on segment p2q2
        if (o4 == 0 && onSegment(p2, q1, q2)) return true;
  
        return false; // Doesn't fall in any of the above cases
    }
  
}