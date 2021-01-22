//
// Created by Wenyou Wang on 1/7/21.
//

#ifndef SIMPLEPHYSICSENGINE_SHAPE_H
#define SIMPLEPHYSICSENGINE_SHAPE_H

#include "../common/common.h"

class sPE_object;

class AABB {
public:
    float x0, y0, x1, y1;
    AABB(float x0, float y0, float x1, float y1) : x0(x0), y0(y0), x1(x1), y1(y1) {}
    AABB() {}
    void Enlarge(float w = 1.0) {
        x0 -= w; y0 -= w;
        x1 += w; y1 += w;
    }
    AABB &operator += (const AABB &other) {
        x0 = std::min(x0, other.x0);
        y0 = std::min(y0, other.y0);
        x1 = std::max(x1, other.x1);
        y1 = std::max(y1, other.y1);

    }
    AABB &operator += (const Vector2f &vec) {
        x0 = std::min(x0, vec.x);
        y0 = std::min(y0, vec.y);
        x1 = std::max(x1, vec.x);
        y1 = std::max(y1, vec.y);
    }
    bool Overlap(const AABB &other) {
        return (!(x1 < other.x0 || other.x1 < x0)) && (!(y1 < other.y0 || other.y1 < y0));
    }
};

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
    Vector2f edges[maxPolygenVertex];
    Vector2f normals[maxPolygenVertex];


public:

    ~sPE_shape(){destroy();}
    void  destroy()
    {

    };


    void  initialize(uint vCount, Vector2f *vArray, bool isFixed = false);
    void  setVertexCount(int count){vertextCount = count; }
    uint  getVertexCount(){return vertextCount;}

    Vector2f*  getVertices(){return vertices; }
    Vector2f*  getNormals(){return normals; }

    void setDensity(float rho){density = rho;}
    //we assume the mass is evenly distributed.
    void calculateMassAndInertia(bool isFixed);

    inline float getMass(){return mass;}
    inline float getInvMass(){return invMass;}

    inline float getInertia(){return inertia;}
    inline float getInvInertia(){return invInertia;}





};

//default shape
sPE_shape* generateBox();

sPE_shape* generateTriangle();

sPE_shape* generateGround();




#endif //SIMPLEPHYSICSENGINE_SHAPE_H
