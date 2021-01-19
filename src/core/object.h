//
// Created by 王文优 on 1/6/21.
//

#ifndef SIMPLEPHYSICSENGINE_OBJECT_H
#define SIMPLEPHYSICSENGINE_OBJECT_H

#include "../common/common.h"
#include "shape.h"


class sPE_Draw;

class sPE_object
{
public:
    sPE_object(){};
    void  initialize(Vector2f posW, float rotationAngle, bool isfixed = false);


    ~sPE_object(){destroy();}
    void  destroy()
    {
        delete shape;
    };

    //integrate a single time step
    void step(float timeStep);
    // update the transformMatrix
    void updateTransform();
    // push the date into opengl buffers
    void prepareRenderingData(sPE_Draw *scene);

    void applyImpulse(Vector2f pos,Vector2f p);

    void applyTorque(double torque);

    void appleGravityForce(float t);

private:
    bool fixed;
    sPE_shape* shape;
    Vector2f posWorld, linearVelocity;
    float rotationAngle, angularVelocity;

    Matrix3x3 transformToWorld, transformToWorldInverse;

};



#endif //SIMPLEPHYSICSENGINE_OBJECT_H
