//
// Created by 王文优 on 1/6/21.
//

#ifndef SIMPLEPHYSICSENGINE_OBJECT_H
#define SIMPLEPHYSICSENGINE_OBJECT_H

#include "../common/common.h"


class sPE_Draw;

class sPE_object
{
public:
    void  initialize(Vector2f posW, float rotationAngle);

    //integrate a single time step
    void step(float timeStep);
    // update the transformMatrix
    void updateTransform();
    // push the date into opengl buffers
    void prepareRenderingData(sPE_Draw *scene);

private:
    float mass, invMass;
    float inertia, invInertia;

    Vector2f posWorld, linearVelocity;
    float rotationAngle, angularVelocity;

    Vector2f vertices[4];

    Matrix3x3 transformToWorld, transformToWorldInverse;







};



#endif //SIMPLEPHYSICSENGINE_OBJECT_H
