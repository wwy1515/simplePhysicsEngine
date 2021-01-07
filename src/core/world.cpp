//
// Created by 王文优 on 1/7/21.
//
#include "world.h"
#include "../render/draw.h"


void sPE_world::initialize(sPE_Draw *pScene)
{
    scene = pScene;
}
void sPE_world::step()
{
    for(int i=0;i<objects.size();i++)
    {
        objects.at(i)->step(timeInterval);
    }

}

void sPE_world::prepareRenderingData()
{
    for(int i=0;i<objects.size();i++)
    {
        objects.at(i)->prepareRenderingData(scene);
    }

}

