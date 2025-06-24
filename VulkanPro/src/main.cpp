#include <stdexcept>

#include "TriangleFunc.h"

int main()
{
    TriangleFunc app;
    try {
        app.Run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}