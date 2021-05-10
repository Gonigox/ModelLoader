
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

    class Render_Task
    {
    private:

        typedef Rgb888                Color;
        typedef Color_Buffer< Color > Color_Buffer;

    private:
        
        unsigned                   width;
        unsigned                   height;
        Color_Buffer               color_buffer;

        Rasterizer< Color_Buffer > rasterizer;
        Scene                      *scene;

        float                      divisor;

    public:

        Render_Task(unsigned width, unsigned height, std::string name, Scene * scene);

        void Render_game();

    private:

        bool  is_frontface (const Point4f * const projected_vertices, const int * const indices);

        bool  is_inside_frustrum (const Point4i* const display_vertices, const int * const indices);

        Point3f calculate_normal    (const Point4f & a, const Point4f & b, const Point4f & c); 

        float   calculate_intensity (const Point3f & normal_vector, const Point3f & ligth_vector);

        Color   calculate_light     (
                                        const Point4f * const projected_vertices, 
                                        const Color   *       original_color, 
                                              Point3f         ligth_vector, 
                                        const int     * const indices
                                    );

        int     clip_polygon        (
                                       Point4i   *       input_vertices, 
                                       const int * const first, 
                                       const int * const last, 
                                       Point4i   *       clipped_vertices, 
                                       int               viewport_width, 
                                       int               viewport_height
                                    );

        int     clip_polygon_side   (
                                        Point4i   * input_vertices,
                                        const int * first,
                                        const int * last,
                                        Point4i   * clipped_vertices, 
                                        Point4i     clip_line_origin, 
                                        Point4i     clip_line_end
                                    );

        Point4i lineLineIntersection(Point4i A, Point4i B, Point3i C, Point3i D);

    };

}
