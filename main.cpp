#include "pch.h"

int main(void)
{
    CollisionConfig* config = new CollisionConfig(10000, ESearchType::KD_TREE, EThreadMode::SINGLE);
    config->Run();

    delete config;
    config = nullptr;
    return 0;
}