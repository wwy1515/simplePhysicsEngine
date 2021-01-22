//
// Created by Wenyou Wang on 1/22/21.
//

#ifndef SIMPLEPHYSICSENGINE_BROADPHASE_H
#define SIMPLEPHYSICSENGINE_BROADPHASE_H
#include "object.h"
using namespace  std;

class IntersectionTest {

    vector<sPE_object *> objects;
    int n;
    AABB aabbs[64];
    float skin;

public:
    IntersectionTest() {}
    void Init(vector<sPE_object *> &pObjects, float skin) {
        objects.clear();
        for (vector<sPE_object *>::iterator it = pObjects.begin(); it != pObjects.end(); it++)
        {
           objects.push_back(*it);
        }

        this->skin = skin;
    }
    vector<pair<sPE_object *, sPE_object *> > getResult() {

        n = (int)objects.size();

        for (int i = 0; i < n; i++)
        {
            aabbs[i] = objects[i]->getAABB();
            aabbs[i].Enlarge(skin);
        }
        vector<pair<sPE_object *, sPE_object *> > ret;
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                if (aabbs[i].Overlap(aabbs[j]))
                {
                    ret.push_back(make_pair(objects[i], objects[j]));
                    logger.log(logType::key,"detectBroadPhaseCollision",timer.getTime());
                }
            }
        }
        return ret;
    }
};

#endif //SIMPLEPHYSICSENGINE_BROADPHASE_H
