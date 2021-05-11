
// Este código es de dominio público.
// gonigox@gmail.com
// 2021.04

#pragma once
#include <Render_Task.hpp>
#include <Model.hpp>
#include <SFML/Window.hpp>
#include <Camera.hpp>

namespace model_view
{
    /// <summary>
    /// Class Scene, creates the window of SFML, stores the camera the ligth and a vector containing the models
    /// </summary>
    class Scene
    {
    private:

        /// <summary>
        /// SFML window
        /// </summary>
        sf::Window  window;
        /// <summary>
        /// Render_task in cahrge of rendering all the models inside the scene
        /// </summary>
        Render_Task render_task;

    public:

        /// <summary>
        /// Vector of the global ligth in the scene
        /// </summary>
        Vector3f                      Ligth;
        /// <summary>
        /// Camera of the scene
        /// </summary>
        Camera                        camera;
        /// <summary>
        /// List of all the models inside the scene
        /// </summary>
        vector< shared_ptr< Model > > model_list;

    public:

        /// <summary>
        /// Constructor from class Scene
        /// </summary>
        /// <param name="width"> Width of the window </param>
        /// <param name="height"> Heigth of the window </param>
        /// <param name="name"> Name of the window </param>
        /// <param name="ligth"> Vector of the global ligth you want in the scene </param>
        Scene(unsigned width, unsigned height, std::string name, Vector3f ligth);

        /// <summary>
        /// Updates the models per frame of the game
        /// </summary>
        void update_game();

        /// <summary>
        /// Starts the main loop of the SFML window
        /// </summary>
        void start_game();

        
    };
}