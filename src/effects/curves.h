#ifndef CURVES_H
#define CURVES_H

#include <AE_Effect.h>
#include "../plugin.h"

class CurvesEffect {
public:
    static PF_Err interpolateCurve(const CurveDef& curve, PF_FpLong input, PF_FpLong& output);
    static PF_Err applyCurves(CrazyCurvesParams* params, PF_Pixel8* input, PF_Pixel8* output);
private:
    static PF_FpLong bezierInterpolate(PF_FpLong p0, PF_FpLong p1, PF_FpLong p2, PF_FpLong p3, PF_FpLong t);
};

#endif // CURVES_H