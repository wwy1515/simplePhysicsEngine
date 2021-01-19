//
// Created by 王文优 on 1/6/21.
//
#include "object.h"
#include "shape.h"
#include "../render/draw.h"

void sPE_object::initialize(Vector2f posW, float rotationA, bool isfixed)
{
    fixed = isfixed;
    posWorld = posW;
    rotationAngle = rotationA;
    angularVelocity = sPE_M_PI;
    linearVelocity  = Vector2f(0.0f,0.0f);

    shape = generateBox();

    updateTransform();
}

void sPE_object::step(float timeStep)
{
    float subTimeStep = timeStep * 0.1f;
    for(uint i = 0; i < 10; i++)
    {
        rotationAngle += subTimeStep * angularVelocity;
        posWorld += subTimeStep * linearVelocity;
        updateTransform();
    }
    appleGravityForce(timeStep);
}

void sPE_object::updateTransform()
{
    transformToWorld.SetAsAxisTransform(posWorld, rotationAngle);
    transformToWorldInverse.SetAsAxisTransform(posWorld, rotationAngle);
}

void sPE_object::prepareRenderingData(sPE_Draw *scene)
{
    sPE_Color drawColor(1.0f,0.0f,0.0f);
    uint vertexCount = shape->getVertexCount();

    Vector2f transformedVertices[maxPolygenVertex];
    for(int i = 0;i < vertexCount;i++)
    {
        transformedVertices[i].Set(shape->getVertices()[i].Rotate(rotationAngle) + posWorld);
    }
    scene->DrawPolygon(transformedVertices,vertexCount,drawColor);

}

void sPE_object::appleGravityForce(float t)
{
    if(fixed)
        return;
    applyImpulse(posWorld, shape->getMass() * t * Vector2f(0.0f,gravity));
}

void sPE_object::applyTorque(double torque)
{
    angularVelocity += torque * shape->getInvInertia();

}

void sPE_object::applyImpulse(Vector2f pos, Vector2f p)
{
    linearVelocity += shape->getInvMass() * p;
    applyTorque(Cross((pos - posWorld), p));
}


