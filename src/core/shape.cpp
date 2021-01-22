//
// Created by Wenyou Wang on 1/7/21.
//
#include "shape.h"

void sPE_shape::initialize(uint vCount, Vector2f *vArray, bool isFixed)
{
    vertextCount = vCount;
    edgecount = vertextCount;
    for(int i = 0; i < vertextCount; i++)
    {
        vertices[i].Set(vArray[i]);
    }

    calculateMassAndInertia(isFixed);

    for(int i = 0; i < edgecount; i++)
    {
        edges[i].Set(vArray[(i+1) % vertextCount] - vArray[i]);
    }

    for(int i = 0; i < edgecount; i++)
    {
        Vector2f tangent = edges[i].tangent();
        tangent.Normalize();
        if(Dot(tangent, vertices[i] - centroidPosition) < 0.0f)
            tangent = -tangent;
        normals[i].Set(tangent);
    }





}

void sPE_shape::calculateMassAndInertia(bool isFixed)
{
    setDensity(default_density);


    if(isFixed)
    {
        mass = sPE_Inf;
        invMass = 0.0f;
    }
    else
    {
        float massSum = 0.0f;
        for(int i = 1 ;i < edgecount; i++)
        {
            Vector2f a = getVertices()[i] - getVertices()[0];
            Vector2f b = getVertices()[i+1] - getVertices()[0];
            massSum += triangleArea(a,b) * default_density;
        }
        mass = massSum;
        invMass = 1.0f / mass;

    }

    Vector2f newCenter = Vector2f(0.0f, 0.0f);
    for (int i = 0; i < vertextCount; i++)
    {
        newCenter += 0.5f * (Cross(vertices[i], vertices[(i + 1) % vertextCount]) * (vertices[i] + vertices[(i + 1) % vertextCount]) );
    }

    newCenter /= (mass / density) * 3;
    centroidPosition = newCenter;


    if(isFixed)
    {
        inertia = sPE_Inf;
        invInertia = 0.0f;

    }
    else
    {
        inertia = 0.0f;
        for (int i = 0; i < vertextCount; i++)
        {
            Vector2f a = vertices[i];
            Vector2f b = vertices[(i + 1) % vertextCount];

            inertia += (a.x * b.y - b.x * a.y) *
                       (a.x * a.x + b.x * b.x + a.x * b.x + a.y * b.y + a.y * a.y + b.y * b.y);
        }
        inertia *= density;
        inertia /= -12.0f;

        invInertia = 1.0f / inertia;
    }




}



sPE_shape* generateBox()
{
    sPE_shape* shape = new sPE_shape();
    Vector2f vertices[4];

    vertices[0].Set(-3.0, -3.0f);
    vertices[1].Set(-3.0, 3.0f);
    vertices[2].Set(3.0, 3.0f);
    vertices[3].Set(3.0, -3.0f);

    shape->initialize(4, vertices);

    return shape;
}

sPE_shape* generateTriangle()
{
    sPE_shape* shape = new sPE_shape();
    Vector2f vertices[3];

    vertices[0].Set(-3.0, 3.0f);
    vertices[1].Set(3.0, 3.0f);
    vertices[2].Set(3.0, -3.0f);

    shape->initialize(3, vertices);

    return shape;
}

sPE_shape* generateGround()
{
    sPE_shape* shape = new sPE_shape();
    Vector2f vertices[4];

    vertices[0].Set(-40.0, -1.0f);
    vertices[1].Set(-40.0, 1.0f);
    vertices[2].Set(40.0, 1.0f);
    vertices[3].Set(40.0, -1.0f);

    shape->initialize(4, vertices,true);

    return shape;
}

