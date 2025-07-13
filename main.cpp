#include "pch.h"

int main(void)
{
    CollisionConfig* config = new CollisionConfig(100000, ESearchType::ARRAY, EThreadMode::SINGLE);
    config->Run();

    delete config;
    config = nullptr;
    return 0;
}