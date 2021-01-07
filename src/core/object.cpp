//
// Created by 王文优 on 1/6/21.
//
#include "object.h"
#include "../render/draw.h"

void sPE_object::initialize(Vector2f posW, float rotationA)
{
    posWorld = posW;
    rotationAngle = rotationA;
    angularVelocity = 2.0f * sPE_M_PI;

    vertices[0].Set(-1.0, -1.0f);
    vertices[1].Set(-1.0, 1.0f);
    vertices[2].Set(1.0, 1.0f);
    vertices[3].Set(1.0, -1.0f);

    updateTransform();
}

void sPE_object::step(float timeStep)
{
    rotationAngle += timeStep * sPE_M_PI;
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

    Vector2f transformedVertices[4];
    for(int i = 0;i < 4;i++)
    {
        transformedVertices[i].Set(vertices[i].Rotate(rotationAngle) + posWorld);
    }
    scene->DrawPolygon(transformedVertices,4,drawColor);

}

