//#include <Scene.hpp>
//
//namespace example
//{
//    Scene::Scene (unsigned width, unsigned height, std::string name)
//    :
//        render_task (width, height, name, this),
//        window      (sf::VideoMode(width, height), name, sf::Style::Titlebar | sf::Style::Close)
//    {
//
//    }
//
//    void Scene::start_game()
//    {
//        bool exit = false;
//
//        do
//        {
//            sf::Event event;
//
//            while (window.pollEvent (event))
//            {
//                if (event.type == sf::Event::Closed) exit = true;
//            }
//
//            update_game();
//            
//            render_task.Render_game();
//
//            window.display ();
//        }
//        while (not exit);
//    }
//}