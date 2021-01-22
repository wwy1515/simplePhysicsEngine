//
// Created by Wenyou Wang on 1/6/21.
//
#include "object.h"
#include "shape.h"
#include "../render/draw.h"

void sPE_object::initialize(sPE_shape* pShape,Vector2f posW, float rotationA, bool isfixed)
{
    fixed = isfixed;
    posWorld = posW;
    rotationAngle = rotationA;
    angularVelocity = 0.0f;
    linearVelocity  = Vector2f(0.0f,0.0f);
    shape = pShape;

    correctiveLinearVelocity = Vector2f(0.0f, 0.0f);
    correctiveAngularVelocity = 0.0f;

    friction = 0.1f;
    restitution = 0.66f;

    updateTransform();
}

void sPE_object::step(float timeStep)
{
    if(fixed) return;
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

    uint vertexCount = shape->getVertexCount();
    for(int i = 0;i < vertexCount;i++)
    {
        curVertices[i].Set(shape->getVertices()[i].Rotate(rotationAngle) + posWorld);
    }
    for(int i = 0;i < vertexCount;i++)
    {
        curNormals[i].Set(shape->getNormals()[i].Rotate(rotationAngle));
    }

}

void sPE_object::prepareRenderingData(sPE_Draw *scene)
{
    sPE_Color drawColor(1.0f,0.0f,0.0f);
    uint vertexCount = shape->getVertexCount();
    scene->DrawPolygon(curVertices,vertexCount,drawColor);

}

void sPE_object::appleGravityForce(float t)
{
    if(fixed)
        return;
    applyImpulse(posWorld, shape->getMass() * t * Vector2f(0.0f,gravity));
}

void sPE_object::applyTorque(float torque)
{
    angularVelocity += torque * shape->getInvInertia();

}

void sPE_object::applyImpulse(Vector2f pos, Vector2f p)
{
    linearVelocity += shape->getInvMass() * p;
    applyTorque(Cross((pos - posWorld), p));
}

void sPE_object::applyPositionCorrection(float timeStep)
{
    if (fixed) return;
    posWorld += timeStep * correctiveLinearVelocity;
    rotationAngle += timeStep * correctiveAngularVelocity;
    correctiveLinearVelocity = Vector2f(0.0f, 0.0f);
    correctiveAngularVelocity = 0.0f;
    updateTransform();
}

void sPE_object::applyCorrectiveImpulse(Vector2f r, Vector2f p, bool paintNeed)
{
    correctiveLinearVelocity += shape->getInvMass() * p;
    correctiveAngularVelocity += shape->getInvInertia() * Cross(r - posWorld, p);
}

Vector2f sPE_object::getPointVelocity(Vector2f p)
{
    if (fixed) return Vector2f(0.0f, 0.0f);
    else return linearVelocity + Vector2f(-angularVelocity * (p.y - posWorld.y), angularVelocity * (p.x - posWorld.x));
}

void sPE_object::getProjection(const Vector2f &normal, float &minI, float &maxI) {

    minI = maxI = Dot(curVertices[0] , normal);
    for (int i = 1; i < shape->getVertexCount(); i++) {
        float prod = Dot(curVertices[i] , normal);
        if (prod < minI) minI = prod;
        if (prod > maxI) maxI = prod;
    }
}

AABB sPE_object::getAABB()
{
    AABB ret;
    getProjection(Vector2f(1.0f, 0.0f), ret.x0, ret.x1);
    getProjection(Vector2f(0.0f, 1.0f), ret.y0, ret.y1);
    return ret;
}


