#include "application.h"
#include "log.h"
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 3)
        {
            std::cout << "please specify address and port" << std::endl;
            return 1;
        }

        App().Run(argv[1], atoi(argv[2]));
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(e.what());
    }

    return 0;
}