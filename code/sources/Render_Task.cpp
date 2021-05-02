#include <Render_Task.hpp>
//#include <Scene.hpp>
//
//namespace example
//{
//    Render_Task::Render_Task(unsigned width, unsigned height, std::string name, Scene * scene)
//    :
//        width        (width ),
//        height       (height),
//        color_buffer (width, height),
//        rasterizer   (color_buffer )
//    {
//        this->scene = scene;
//        divisor = divisor = 1.f / ( pow(2, (Color::bits / Color::component_count)) - 1 );
//    }
//
//    void Render_Task::Render_game()
//    {
//        // Se convierten las coordenadas transformadas y proyectadas a coordenadas
//        // de recorte (-1 a +1) en coordenadas de pantalla con el origen centrado.
//        // La coordenada Z se escala a un valor suficientemente grande dentro del
//        // rango de int (que es lo que espera fill_convex_polygon_z_buffer).
//        
//        for(size_t model_num = 0; model_num < scene->model_list.size(); ++model_num)
//        {
//            shared_ptr< Model > model = scene->model_list[model_num];            
//
//            for(size_t mesh_num = 0; mesh_num < model->mesh_list.size(); ++mesh_num)
//            {
//                shared_ptr< Mesh > mesh = model->mesh_list[mesh_num];
//
//                Matrix44 identity(1);
//                Matrix44 scaling        = scale (identity, float(width/ 2), float(height / 2), 100000000.f);
//                Matrix44 translation    = translate (identity, Vector3f{ float(width / 2), float(height / 2), 0.f });
//                Matrix44 transformation = translation * scaling;
//                
//
//                for (size_t index = 0, number_of_vertices = mesh->transformed_vertices.size (); index < number_of_vertices; index++)
//                {
//                    mesh->display_vertices[index] = Point4i( transformation * mesh->transformed_vertices[index] );            
//                }       
//
//                // Se borra el framebúffer y se dibujan los triángulos:
//
//                rasterizer.clear ();
//
//                for 
//                (
//                    int * indices = mesh->original_indices.data (), * end = indices + mesh->original_indices.size (); 
//                    indices < end; 
//                    indices += 3
//                )
//                {
//                    if (is_frontface (mesh->transformed_vertices.data (), indices))
//                    {
//                        // Se calcula el color del triangulo iluminado a partir del primer vertice del mismo
//
//                        Color triangle_color = calculate_light
//                            (
//                                mesh->transformed_vertices.data(), 
//                                mesh->original_colors.data(), 
//                                scene->Ligth, 
//                                indices
//                            );
//                        
//                        // Se establece el color del polígono
//
//                        rasterizer.set_color (triangle_color);
//
//                        // Se rellena el polígono:
//
//                        rasterizer.fill_convex_polygon_z_buffer (mesh->display_vertices.data (), indices, indices + 3);
//                    }
//                }
//            }  
//        }
//        // Se copia el framebúffer oculto en el framebúffer de la ventana:
//
//        color_buffer.blit_to_window ();
//    }
//
//    bool Render_Task::is_frontface(const Point4f * const projected_vertices,const int * const indices)
//    {
//        const Point4f & v0 = projected_vertices[indices[0]];
//        const Point4f & v1 = projected_vertices[indices[1]];
//        const Point4f & v2 = projected_vertices[indices[2]];
//
//        // Se asumen coordenadas proyectadas y polígonos definidos en sentido horario.
//        // Se comprueba a qué lado de la línea que pasa por v0 y v1 queda el punto v2:
//
//        return ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]) < 0.f);
//    }
//
//    Point3f Render_Task::calculate_normal(const Point4f & a,const Point4f & b,const Point4f & c)
//    {
//        Point3f vector1 = b - a;
//        Point3f vector2 = c - a;
//        
//        Point3f normal = glm::cross (vector1, vector2);
//
//        return normal;
//    }
//
//    float Render_Task::calculate_intensity(const Point3f & normal_vector,const Point3f & ligth_vector)
//    {
//        float intensity = glm::dot (normal_vector, ligth_vector);
//            
//        if(intensity > 1)
//        {
//            intensity = 1;
//        }
//        else if(intensity < 0)
//        {
//            intensity = 0;
//        }
//
//        return intensity;
//    }
//
//    Render_Task::Color Render_Task::calculate_light(const Point4f * const projected_vertices,const Color * original_color,Point3f ligth_vector,const int * const indices)
//    {
//        // Se calcula la normal normalizada del triangulo
//
//        Point3f triangle_normal = calculate_normal
//            (
//                projected_vertices[indices[0]], 
//                projected_vertices[indices[1]], 
//                projected_vertices[indices[2]]
//            );
//
//        triangle_normal = glm::normalize(triangle_normal);
//
//        // Con la normal del triangulo se calcula el valor de la intensidad en él
//
//        float triangle_intesity = calculate_intensity(triangle_normal, ligth_vector);
//
//        // Se modifica el color segun la intensidad de luz recibida
//
//        float r = original_color[indices[0]].red()   * divisor;
//        float g = original_color[indices[0]].green() * divisor;
//        float b = original_color[indices[0]].blue()  * divisor;
//
//       Color final_color = Color
//            ( 
//                r * triangle_intesity,  
//                g * triangle_intesity, 
//                b * triangle_intesity
//            );
//
//       return final_color;
//    }
//  
//}