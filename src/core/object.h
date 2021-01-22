//
// Created by Wenyou Wang on 1/6/21.
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

    void  initialize(sPE_shape* pShape,Vector2f posW, float rotationAngle, bool isfixed = false);

    ~sPE_object(){destroy();}
    void  destroy()
    {

    };

    //integrate a single time step
    void step(float timeStep);
    // update the transformMatrix
    void updateTransform();
    // push the date into opengl buffers
    void prepareRenderingData(sPE_Draw *scene);

    void applyImpulse(Vector2f pos,Vector2f p);

    void applyTorque(float torque);

    void applyPositionCorrection(float timeStep);

    void applyCorrectiveImpulse(Vector2f r, Vector2f p, bool paintNeed = true);

    void appleGravityForce(float t);

    void setAngularVelocity(float v){angularVelocity = v; }

    Vector2f getPointVelocity(Vector2f p);

    void getProjection(const Vector2f &normal, float &minI, float &maxI);

    AABB getAABB();

    float getRestitution() {return restitution;}

    float getFriction() {return friction;}

    const lineSegment& getEdge(int i) const {
        return lineSegment(curVertices[i], curVertices[(i+1) % shape->getVertexCount()]);
    }

    sPE_shape* getShape() {return shape;}

    const Vector2f &getNormal(int i) const {
        return curNormals[i];
    }

    const Vector2f &getPoint(int i) const {
        return curVertices[i];
    }

    inline uint  getVertexCount(){return shape->getVertexCount();}

    inline Vector2f getPosWorld() {return posWorld;}

    inline float getRotationEngle() {return rotationAngle;}

    inline float getMass(){return shape->getMass();}
    inline float getInvMass(){return shape->getInvMass();}

    inline float getInertia(){return shape->getInertia();}
    inline float getInvInertia(){return shape->getInvInertia();}


private:
    bool fixed;
    sPE_shape* shape;
    Vector2f posWorld, linearVelocity;
    float rotationAngle, angularVelocity;
    float restitution, friction;

    Vector2f correctiveLinearVelocity;
    float correctiveAngularVelocity;

    Matrix3x3 transformToWorld, transformToWorldInverse;

    Vector2f curVertices[maxPolygenVertex];
    Vector2f curNormals[maxPolygenVertex];

};



#endif //SIMPLEPHYSICSENGINE_OBJECT_H
