//
// Created by Wenyou Wang on 1/22/21.
//

#ifndef SIMPLEPHYSICSENGINE_CONSTRAINTS_H
#define SIMPLEPHYSICSENGINE_CONSTRAINTS_H

#include "../common/common.h"
#include "object.h"

class Constraint{
public:
    int type;
    Constraint() {type = 0;}
    virtual void ProcessVelocity() = 0;
    virtual void ProcessPosition() = 0;
};

class contactConstraint : public Constraint
{
public:
    sPE_object *objectA, *objectB;
    Vector2f p, n;
    float restitution, friction;
    float depth;

    contactConstraint(sPE_object *pObjectA, sPE_object *pObjectB, Vector2f p, Vector2f n, float depth)
    : objectA(pObjectA), objectB(pObjectB),p(p), n(n), depth(depth) {
        restitution = sqrt(pObjectA->getRestitution() * pObjectA->getRestitution());
        friction = sqrt(pObjectA->getFriction() * pObjectA->getFriction());
        type = 1;
    }

    void ProcessVelocity()
    {
        Vector2f v10 = objectB->getPointVelocity(p) - objectA->getPointVelocity(p);
        Vector2f r0 = p - objectA->getPosWorld(), r1 = p - objectB->getPosWorld();
        float v0 = -Dot(n, v10);

        Vector2f tao = n.tangent();
        float cRestitution;
        if (v0 > 1.0f) cRestitution = restitution;
        else cRestitution = 0.0f;
        float J = ((1 + cRestitution) * v0) /
                   (objectA->getInvMass() + objectB->getInvMass() +
                   sqr(Cross(r0, n)) * objectA->getInvInertia() +
                   sqr(Cross(r1, n)) * objectB->getInvInertia());
        if (J < 0.0f) return;
        Vector2f impulse = J * n;
        objectA->applyImpulse(p, -impulse);
        objectB->applyImpulse(p, impulse);
        //if (settings.frictionSwitch)
        {
            v10 = objectB->getPointVelocity(p) - objectA->getPointVelocity(p);
            float j = -Dot(v10, tao) /
                    (objectA->getInvMass() + objectB->getInvMass() +
                    sqr(Cross(r0, tao)) * objectA->getInvInertia() +
                    sqr(Cross(r0, tao)) * objectB->getInvInertia());
            j = std::max(std::min(j, friction * J), -friction * J);
            Vector2f fImpulse = j * tao;
            objectA->applyImpulse(p, -fImpulse);
            objectB->applyImpulse(p, fImpulse);
        }

    }

    void ProcessPosition()
    {
        Vector2f r0 = p - objectA->getPosWorld(), r1 = p - objectB->getPosWorld();
        float correctiveJ = (0.5 * std::max(0.0, depth - 0.01) / timeInterval) /
                             (objectA->getInvMass() + objectB->getInvMass() +
                             sqr(Cross(r0, n)) * objectA->getInvInertia() +
                             sqr(Cross(r1, n)) * objectB->getInvInertia());
        if (correctiveJ > 0.0f) {
            objectA->applyCorrectiveImpulse(p, -correctiveJ * n, true);
            objectB->applyCorrectiveImpulse(p, correctiveJ * n, true);
        }
    }

};
#endif //SIMPLEPHYSICSENGINE_CONSTRAINTS_H
