
// Este código es de dominio público.
// angel.rodriguez@esne.edu
// 2021.12 - 2021.04

#include <string>
#include <Model.hpp>
#include <Scene.hpp>

using namespace example;
using namespace std;

int main ()
{
    constexpr auto window_width  = 800u;
    constexpr auto window_height = 600u;


    unique_ptr < Scene > scene  = make_unique < Scene > (window_width, window_height, "Mesh_Test", Vector3f {1.f, -1.f, -0.35f});

    //low-poly-mill.dae
    //cube.obj
    //monkey.obj

#ifdef _DEBUG
    shared_ptr < Model > cube   = make_shared < Model > ("../../Binaries/Assets/low-poly-mill.dae");
#endif // DEBUG
#ifdef NDEBUG
    shared_ptr < Model > cube   = make_shared < Model > ("Assets/low-poly-mill.dae");
#endif // !NDEBUG

    //scene->model_list.push_back(cube);
    scene->model_list.push_back(cube);

    scene->start_game();

    return 0;
}
