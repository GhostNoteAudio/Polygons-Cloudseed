#include "CloudSeed.h"

void setup()
{
    Polygons::init();
    CloudSeed::start();
}

void loop()
{
    delay(50);
    CloudSeed::loop();
}
