//
// Created by Wenyou Wang on 1/7/21.
//
#include "world.h"
#include "../render/draw.h"
#include "../common/logger.h"



void sPE_world::initialize(sPE_Draw *pScene)
{
    scene = pScene;
}
void sPE_world::step()
{
    logger.log(logType::info,"worldStep",timer.getTime());

    for(int i=0;i<objects.size();i++)
    {
        objects.at(i)->step(timeInterval);
    }


    //broadPhase collision detection
    iTest.Init(objects,1.0);
    vector<pair <sPE_object*, sPE_object *> > potentialCols = iTest.getResult();

    //narrowPhase collision detection
    solver.processVelocityConstraint(potentialCols, constraints);

    iTest.Init(objects, 1.0);
    potentialCols = iTest.getResult();

    solver.processPositionConstraint(potentialCols, constraints, objects);





    //finished delete all contactConstraints
    for (; !constraints.empty(); ) {
        delete constraints.back();
        constraints.pop_back();
    }




}


void sPE_world::prepareRenderingData()
{
    for(int i=0;i<objects.size();i++)
    {
        objects.at(i)->prepareRenderingData(scene);
    }

}

