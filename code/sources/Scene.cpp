#include <Scene.hpp>

namespace example
{
    Scene::Scene (unsigned width, unsigned height, std::string name, Vector3f ligth)
    :
        render_task (width, height, name, this),
        window      (sf::VideoMode(width, height), name, sf::Style::Titlebar | sf::Style::Close),
        camera      ()
    {
        Ligth = ligth;
    }

    void Scene::update_game()
    {
        static float angle_mesh  = 0.f;
        static float angle_model = 0.f;
        angle_mesh  += 0.05f;
        angle_model += 0.01f;

        //model_list[0]->mesh_list[0]->transform_component.rotation = Vector3f{0.f,0.f,  angle_mesh};

        //model_list[0]->transform_component.rotation = Vector3f{0.f, angle_model, 0.f};
        
        for(size_t i = 0; i < model_list.size(); ++i)
        {
            model_list[i]->update();
        }
    }

    void Scene::start_game()
    {
        bool exit = false;

        /*static float angle_camera = 0.f;
        Vector3f     distance = camera.transform_component.position - model_list[0]->transform_component.position;
        float        radius   = std::sqrtf(distance.x * distance.x + distance.y + distance.y + distance.z * distance.z);*/

        do
        {
            sf::Event event;

            while (window.pollEvent (event))
            {
                switch (event.type)
                {
                    // window closed
                    case sf::Event::Closed:
                        window.close();
                        exit = true;
                        break;

                    // key pressed
                    case sf::Event::KeyPressed:
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                        {
                            camera.move_camera(Vector3f {-1.f, 0.f, 0.f} );
                        }
                        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                        {
                            camera.move_camera(Vector3f {1.f, 0.f, 0.f} );
                        }
                        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                        {
                            camera.move_camera(Vector3f {0.f, -1.f, 0.f} );
                        }
                        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                        {
                            camera.move_camera(Vector3f {0.f, 1.f, 0.f} );
                        }
                        break;

                    // we don't process other types of events
                    default:
                        //camera.calculate_mtransformation();
                        break;
                }
            }

            update_game();
            
            render_task.Render_game();

            window.display ();
        }
        while (not exit);
    }
}