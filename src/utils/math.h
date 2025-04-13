#ifndef MATH_H
#define MATH_H

#include <AE_Effect.h>

namespace MathUtils {
    PF_FpLong lerp(PF_FpLong a, PF_FpLong b, PF_FpLong t);
    PF_FpLong clamp(PF_FpLong value, PF_FpLong min, PF_FpLong max);
}

#endif // MATH_H