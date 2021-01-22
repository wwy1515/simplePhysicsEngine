//
// Created by Wenyou Wang on 1/22/21.
//

#include "narrowPhase.h"

void sPE_collisionSolver::processVelocityConstraint(std::vector<std::pair <sPE_object*, sPE_object *> > &potentialCols,std::vector<Constraint *> &constraints)
{
    for(int i = 0 ; i < potentialCols.size();i++)
    {
        polygonPolygonCollision(potentialCols[i].first, potentialCols[i].second, constraints);
    }

    for (int K = 0; K < velocityIteration; K++)
    {
        for (int i = 0; i < (int)constraints.size(); i++)
        {
            constraints[i]->ProcessVelocity();
        }
    }
}

void sPE_collisionSolver::processPositionConstraint(std::vector<std::pair <sPE_object*, sPE_object *> > &potentialCols,std::vector<Constraint *> &constraints, std::vector<sPE_object*> &objects)
{
    for (int K = 0; K < positionIteration; K++)
    {
        for (int i = 0; i < (int)constraints.size(); i++) delete(constraints[i]);
        {
            constraints.clear();
        }

        for (int i = 0; i < (int)potentialCols.size(); i++)
        {
            polygonPolygonCollision(potentialCols[i].first, potentialCols[i].second, constraints);
        }


        for (int i = 0; i < (int)constraints.size(); i++)
        {
            constraints[i]->ProcessPosition();
        }

        for (int i = 0; i < (int)objects.size(); i++)
        {
            objects[i]->applyPositionCorrection(timeInterval);
        }
    }

}

void sPE_collisionSolver::polygonPolygonCollision(sPE_object *obj1, sPE_object *obj2,std::vector<Constraint *> &constraints)
{
    Vector2f n;
    lineSegment edge;
    int edgeId;
    float minDepth = sPE_Inf;
    int flg = 0;

    for (int i = 0; i < obj1->getVertexCount(); i++)
    {
        float min0, max0, min1, max1;
        Vector2f normal = obj1->getNormal(i);
        obj1->getProjection(normal, min0, max0);
        obj2->getProjection(normal, min1, max1);
        if (max0 < min1 || max1 < min0) return;
        float d = max0 - min1;
        if (min1 < max0 && (sgn(d - minDepth) < 0) || sgn(d - minDepth) == 0 && obj1->getMass() > obj2->getMass()) {
            flg = 0;
            minDepth = d;
            n = normal;
            edgeId = i;
            edge = obj1->getEdge(i);
        }
    }


    for (int i = 0; i < obj2->getVertexCount(); i++)
    {
        float min0, max0, min1, max1;
        Vector2f normal = obj2->getNormal(i);
        obj2->getProjection(normal, min0, max0);
        obj1->getProjection(normal, min1, max1);
        if (max0 < min1 || max1 < min0) return;
        double d = max0 - min1;
        if (min1 < max0 && (sgn(d - minDepth) < 0) || sgn(d - minDepth) == 0 && obj2->getMass() > obj1->getMass()) {
            flg = 1;
            minDepth = d;
            n = normal;
            edgeId = i;
            edge = obj2->getEdge(i);
        }
    }


    if (flg)
    {
     std::swap(obj1, obj2);
    }

    flg = -1;
    for (int i = 0; i < obj2->getVertexCount(); i++)
    {
        if (Dot(n ,(obj2->getPoint(i) - edge.a)) < 0.0f && Dot(n , (obj2->getPoint((i + 1) % obj2->getVertexCount()) - edge.a)) < 0.0f)
        {
            flg = i;
            break;
        }
    }

    if (flg != -1)
    {
        // edge-edge collision
        int i = edgeId, j = flg;
        std::pair<float, Vector2f> cons[4];
        Vector2f tan = n.tangent();
        Vector2f A = obj1->getPoint(i),
                 B = obj1->getPoint((i + 1) % obj1->getVertexCount()),
                 C = obj2->getPoint(j),
                 D = obj2->getPoint((j + 1) % obj2->getVertexCount());
        cons[0] = std::make_pair(Dot(tan, A), A);
        cons[1] = std::make_pair(Dot(tan, B), B);
        cons[2] = std::make_pair(Dot(tan, C), C);
        cons[3] = std::make_pair(Dot(tan, D), D);
        std::sort(cons, cons + 4);
        if (sgn(Dot(obj1->getNormal(i), obj2->getNormal(j)) + 1.0) != 0) {
            constraints.push_back(new contactConstraint(obj1, obj2, cons[1].second, n, minDepth));
            constraints.push_back(new contactConstraint(obj1, obj2, cons[2].second, n, minDepth));
        } else {
            constraints.push_back(new contactConstraint(obj1, obj2, 0.5 * (cons[1].second + cons[2].second), n, minDepth));
        }
    }
    else
    {
        //vertex-edge collision
        Vector2f retP;
        double retDep = -sPE_Inf;
        for (int i = 0; i < obj2->getVertexCount(); i++) {
            Vector2f p = obj2->getPoint(i);
            float depth = -Dot((p - edge.a), n);
            if (depth > retDep)
                retDep = depth, retP = p;
        }
        if (retDep > -sPE_epsilon)
            constraints.push_back(new contactConstraint(obj1, obj2, retP, n, retDep));
    }









}

