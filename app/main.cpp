#include "application.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        Application app(argc, argv);
        app.run();
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}