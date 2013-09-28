#include "application.h"


int main(int argc, char* argv[])
{
    try
    {
        App().Run();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(e.what());
    }

    return 0;
}