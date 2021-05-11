
// Este código es de dominio público.
// gonigox@gmail.com
// 2021.04

#pragma once
#include <Rasterizer.hpp>
#include <Color_Buffer.hpp>
#include <string>
#include <math.hpp>

namespace example
{
    using argb::Rgb888;
    using argb::Color_Buffer;

    class Scene;

    /// <summary>
    /// Class in charge of render all the models inside the Scene
    /// </summary>
    class Render_Task
    {
    private:

        typedef Rgb888                Color;
        typedef Color_Buffer< Color > Color_Buffer;

    private:
        
        /// <summary>
        /// Width of the window
        /// </summary>
        unsigned                   width;
        /// <summary>
        /// Heigth of the window
        /// </summary>
        unsigned                   height;
        /// <summary>
        /// Pixel buffer
        /// </summary>
        Color_Buffer               color_buffer;
        /// <summary>
        /// Class in charge of filling the Pixel buffer of the render_task
        /// </summary>
        Rasterizer< Color_Buffer > rasterizer;
        /// <summary>
        /// Pointer to the scene so it can acces the model list to renderize
        /// </summary>
        Scene                      *scene;
        /// <summary>
        /// Divisor to normalize colors when calculating ligth
        /// </summary>
        float                      divisor;

    public:

        /// <summary>
        /// Constructor of the render_task.
        /// The scene is in charge os using it so normaly you shouldn't need to use it
        /// </summary>
        /// <param name="width"> Width of the window </param>
        /// <param name="height"> Heigth of the window </param>
        /// <param name="name"> Name of the window/scene </param>
        /// <param name="scene"> Pointer to the scene wich contain the model list </param>
        Render_Task(unsigned width, unsigned height, std::string name, Scene * scene);


        /// <summary>
        /// Loop from all the models of the Scene and renderize them
        /// </summary>
        void Render_game();

    private:

        /// <summary>
        /// Check if the triangle is oriented to the camera.
        /// It assumes projected coordinates and triangles defined in clock wise order
        /// </summary>
        /// <param name="projected_vertices"> Projected vertices from the model </param>
        /// <param name="indices"> Pointer of the first index of the triangle </param>
        /// <returns></returns>
        bool  is_frontface (const Point4f * const projected_vertices, const int * const indices);

        /// <summary>
        /// Chack if the triangle is inside the frustrum od the camera
        /// </summary>
        /// <param name="display_vertices"> Display vertices of the triangle </param>
        /// <param name="indices"> Pointer of the first index of the triangle </param>
        /// <returns></returns>
        bool  is_inside_frustrum (const Point4i* const display_vertices, const int * const indices);

        /// <summary>
        /// Calculate the normal from a triangle determined by 3 Point3f
        /// </summary>
        /// <param name="a"> Point one of the triangle </param>
        /// <param name="b"> Point two of the triangle </param>
        /// <param name="c"> Point three of the triangle </param>
        /// <returns></returns>
        Point3f calculate_normal    (const Point4f & a, const Point4f & b, const Point4f & c); 

        /// <summary>
        /// Calculate the intensity of the ligth from the Scene on the triangle. Value between 0 and 1
        /// </summary>
        /// <param name="normal_vector"> Normal vector of the triangle </param>
        /// <param name="ligth_vector"> Ligth vector of the Scene </param>
        /// <returns></returns>
        float   calculate_intensity (const Point3f & normal_vector, const Point3f & ligth_vector);

        /// <summary>
        /// Calculate the final color of the first vertex of a triangle after iluminating it
        /// </summary>
        /// <param name="projected_vertices"> Projecter vertices of the model </param>
        /// <param name="original_color"> Original color of the vertex of the triangle </param>
        /// <param name="ligth_vector"> Vector of the ligth </param>
        /// <param name="indices"> Pointer to the first index of the triangle </param>
        /// <returns></returns>
        Color   calculate_light     (
                                        const Point4f * const projected_vertices, 
                                        const Color   *       original_color, 
                                              Point3f         ligth_vector, 
                                        const int     * const indices
                                    );

        /// <summary>
        /// Polygon clipping implementing Sutherlan-Hodgman algorithm
        /// </summary>
        /// <param name="input_vertices"> Original vertices from the triangle, in siplay coordinates </param>
        /// <param name="first"> Pointer to the first index of the triangle </param>
        /// <param name="last"> Pointer to the last index of the triangle + 1 </param>
        /// <param name="clipped_vertices"> Pointer to the array of the new polygon the function creates </param>
        /// <param name="viewport_width"> Window width </param>
        /// <param name="viewport_height"> Window heigth </param>
        /// <returns></returns>
        int     clip_polygon        (
                                       Point4i   *       input_vertices, 
                                       const int * const first, 
                                       const int * const last, 
                                       Point4i   *       clipped_vertices, 
                                       int               viewport_width, 
                                       int               viewport_height
                                    );

        /// <summary>
        /// Clip poligon with a line determined by two Point3i
        /// </summary>
        /// <param name="input_vertices">Polygon to clip </param>
        /// <param name="first"> Pointer to the first index of the triangle </param>
        /// <param name="last"> Pointer to the last index of the triangle + 1 </param>
        /// <param name="clipped_vertices"> Pointer to the array of the new polygon the function creates </param>
        /// <param name="clip_line_origin"> First point of the line </param>
        /// <param name="clip_line_end"> Second point of the line </param>
        /// <returns></returns>
        int     clip_polygon_side   (
                                        Point4i   * input_vertices,
                                        const int * first,
                                        const int * last,
                                        Point4i   * clipped_vertices, 
                                        Point4i     clip_line_origin, 
                                        Point4i     clip_line_end
                                    );

        /// <summary>
        /// Returns the point of intersection between two lines determined by two point each
        /// </summary>
        /// <param name="A"> First point of the line 1</param>
        /// <param name="B"> Second point of the line 1 </param>
        /// <param name="C"> First point of the line 2 </param>
        /// <param name="D"> Second point of the line 2 </param>
        /// <returns></returns>
        Point4i lineLineIntersection(const Point4i & A, const Point4i & B, const Point3i & C, const Point3i & D);

    };

}
