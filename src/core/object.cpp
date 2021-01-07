//
// Created by 王文优 on 1/6/21.
//
#include "object.h"
#include "shape.h"
#include "../render/draw.h"

void sPE_object::initialize(Vector2f posW, float rotationA)
{
    posWorld = posW;
    rotationAngle = rotationA;
    angularVelocity = sPE_M_PI;
    linearVelocity  = Vector2f(0.0f,gravity);

    shape = generateBox();

    updateTransform();
}

void sPE_object::step(float timeStep)
{
    rotationAngle += timeStep * angularVelocity;
    posWorld += timeStep * linearVelocity;
    updateTransform();
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

