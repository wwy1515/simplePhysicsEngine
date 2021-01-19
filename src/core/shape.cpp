//
// Created by 王文优 on 1/7/21.
//
#include "shape.h"

void sPE_shape::initialize(uint vCount, Vector2f *vArray)
{
    vertextCount = vCount;
    for(int i = 0; i < vCount; i++)
    {
        vertices[i].Set(vArray[i]);
    }


    calculateMassAndInertia();


}

void sPE_shape::calculateMassAndInertia()
{
    setDensity(default_density);

    edgecount = vertextCount;
    float massSum = 0.0f;
    for(int i = 1 ;i < edgecount; i++)
    {
        Vector2f a = getVertices()[0] - getVertices()[i];
        Vector2f b = getVertices()[0] - getVertices()[i+1];
        massSum += triangleArea(a,b) * default_density;
    }
    mass = massSum;
    invMass = 1.0f / mass;

}



sPE_shape* generateBox()
{
    sPE_shape* shape = new sPE_shape();
    shape->setVertexCount(4);

    shape->getVertices()[0].Set(-1.0, -1.0f);
    shape->getVertices()[1].Set(-1.0, 1.0f);
    shape->getVertices()[2].Set(1.0, 1.0f);
    shape->getVertices()[3].Set(1.0, -1.0f);

    shape->calculateMassAndInertia();

    return shape;
}

