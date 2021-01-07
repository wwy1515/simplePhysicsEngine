//
// Created by 王文优 on 1/2/21.
//

#ifndef SIMPLEPHYSICSENGINE_MACRO_H
#define SIMPLEPHYSICSENGINE_MACRO_H

#include "header.h"

//macro
#define sPE_epsilon 1e-6f

//type alias
using uint = uint32_t;

//function alias
#define sPE_Sqrt(x) std::sqrt(x)
#define sPE_isValid(x) std::isfinite(x)
#define sPE_Assert(x)  assert(x)
#define cos(x) std::cos(x)
#define sin(x) std::sin(x)
#define sPE_INF std::numeric_limits<double>::infinity()

const float sPE_M_PI = std::acos(-1.);
const float timeInterval = 1.0f/60.0f;


#endif //SIMPLEPHYSICSENGINE_MACRO_H
