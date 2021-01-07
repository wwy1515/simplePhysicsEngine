//
// Created by 王文优 on 1/6/21.
//

#ifndef SIMPLEPHYSICSENGINE_WORLD_H
#define SIMPLEPHYSICSENGINE_WORLD_H
#include "../common/common.h"
#include "object.h"

class sPE_world
{
public:
    sPE_world(){};
    void  initialize(sPE_Draw* pScene);


    ~sPE_world(){destroy();}
    void  destroy()
    {

    };

    void step();
    void prepareRenderingData();
    void pushObject(sPE_object* pObject){objects.push_back(pObject);}

private:
    std::vector<sPE_object*> objects;
    sPE_Draw *scene;


};

#endif //SIMPLEPHYSICSENGINE_WORLD_H
