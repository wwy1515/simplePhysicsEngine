//
// Created by 王文优 on 1/7/21.
//

#ifndef SIMPLEPHYSICSENGINE_SHAPE_H
#define SIMPLEPHYSICSENGINE_SHAPE_H

#include "../common/common.h"

class sPE_object;

class sPE_shape
{
private:
    Vector2f centroidPosition;

    float mass, invMass;
    float inertia, invInertia;

    float density;
    uint edgecount;

    uint vertextCount;
    Vector2f vertices[maxPolygenVertex];
public:
    void  initialize(uint vCount, Vector2f *vArray);
    void  setVertexCount(int count){vertextCount = count; }
    uint  getVertexCount(){return vertextCount;}
    Vector2f*  getVertices(){return vertices; }

    void setDensity(float rho){density = rho;}
    //we assume the mass is evenly distributed.
    void calculateMassAndInertia();

    inline float getMass(){return mass;}
    inline float getInvMass(){return invMass;}

    inline float getInertia(){return inertia;}
    inline float getInvInertia(){return invInertia;}





};

//default shape
sPE_shape* generateBox();




#endif //SIMPLEPHYSICSENGINE_SHAPE_H
