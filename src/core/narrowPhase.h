//
// Created by Wenyou Wang on 1/22/21.
//

#ifndef SIMPLEPHYSICSENGINE_NARROWPHASE_H
#define SIMPLEPHYSICSENGINE_NARROWPHASE_H

#include "../common/common.h"
#include "object.h"
#include "constraints.h"


class sPE_collisionSolver
{
public:
    void polygonPolygonCollision(sPE_object* obj1, sPE_object * obj2,std::vector<Constraint *> &constraints);
    void processVelocityConstraint(std::vector<std::pair <sPE_object*, sPE_object *> > &potentialCols, std::vector<Constraint *> &constraints);
    void processPositionConstraint(std::vector<std::pair <sPE_object*, sPE_object *> > &potentialCols, std::vector<Constraint *> &constraints, std::vector<sPE_object*> &objects);

private:
    int velocityIteration = 40;
    int positionIteration = 10;

};




#endif //SIMPLEPHYSICSENGINE_NARROWPHASE_H
