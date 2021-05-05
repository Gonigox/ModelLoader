#include <Scene.hpp>

namespace example
{
    Scene::Scene (unsigned width, unsigned height, std::string name, Vector3f ligth)
    :
        render_task (width, height, name, this),
        window      (sf::VideoMode(width, height), name, sf::Style::Titlebar | sf::Style::Close)
    {
        Ligth = ligth;
    }

    void Scene::update_game()
    {
        for(size_t i = 0; i < model_list.size(); ++i)
        {
            model_list[i].get()->update();
        }
    }

    void Scene::start_game()
    {
        bool exit = false;

        do
        {
            sf::Event event;

            while (window.pollEvent (event))
            {
                if (event.type == sf::Event::Closed) exit = true;
            }

            update_game();
            
            render_task.Render_game();

            window.display ();
        }
        while (not exit);
    }
}