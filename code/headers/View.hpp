
// Este código es de dominio público.
// angel.rodriguez@esne.edu
// 2013.12 - 2021.04

#ifndef VIEW_HEADER
#define VIEW_HEADER

    #include <Color_Buffer.hpp>
    #include <cstdlib>
    #include <math.hpp>
    #include <Rasterizer.hpp>
    #include <vector>

    namespace example
    {

        using  std::vector;
        using argb::Rgb888;
        using argb::Color_Buffer;

        class View
        {
        private:

            typedef Rgb888                Color;
            typedef Color_Buffer< Color > Color_Buffer;
            typedef Point4f               Vertex;
            typedef vector< Vertex >      Vertex_Buffer;
            typedef vector< int    >      Index_Buffer;
            typedef vector< Color  >      Vertex_Colors;

        private:
#ifdef _DEBUG
            static constexpr char mesh_file_path[] = "../../Binaries/Assets/monkey.obj";
#endif // _DEBUG
#ifdef NDEBUG
            static constexpr char mesh_file_path[] = "Assets/monkey.obj";
 #endif // NDEBUG


            

            Color_Buffer               color_buffer;
            Rasterizer< Color_Buffer > rasterizer;

            Point3f           Ligth;
            float             divisor;

            Vertex_Buffer     original_vertices;
            Index_Buffer      original_indices;
            Vertex_Colors     original_colors;
            Vertex_Buffer     transformed_vertices;
            vector< Point4i > display_vertices;

            unsigned width;
            unsigned height;

        public:

            View(unsigned width, unsigned height);

            void update ();
            void render ();

        private:

            bool  is_frontface (const Vertex * const projected_vertices, const int * const indices);
            float rand_clamp   () { return float(rand () & 0xff) * 0.0039215f; }


            Point3f calculate_normal    (const Vertex & a, const Vertex & b, const Vertex & c); 
            float   calculate_intensity (const Point3f & normal_vector, const Point3f & ligth_vector);
            Color   calculate_light     (const Vertex * const projected_vertices, const Color * original_color, Point3f ligth_vector, const int * const indices);

        };

    }

#endif
