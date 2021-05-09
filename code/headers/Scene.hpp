
// Este código es de dominio público.
// gonigox@gmail.com
// 2021.04

#pragma once
#include <Render_Task.hpp>
#include <Model.hpp>
#include <SFML/Window.hpp>
#include <Camera.hpp>

namespace example
{
    class Scene
    {
    private:

        sf::Window  window;
        Render_Task render_task;

    public:

        Vector3f                      Ligth;
        Camera                        camera;
        vector< shared_ptr< Model > > model_list;

    public:

        Scene(unsigned width, unsigned height, std::string name, Vector3f ligth);

        void update_game();

        void start_game();

        
    };
}