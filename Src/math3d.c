#include "../Inc/math3d.h"

#ifdef LIB3D_USE_FIXED_POINT_ARITHMETIC
fxp_t floatingToFixed( flp_t num ){
    return (fxp_t)( num * (flp_t)( 1 << FP_DP ) + ( num >= 0 ? 0.5 : -0.5 ) );
}
flp_t fixedToFloating( fxp_t num ){
    return (flp_t)(num) / (flp_t)( 1 << FP_DP );
}
fxp_t fixedMul( fxp_t a, fxp_t b ){
    return ( (fxp2_t)(a) * (fxp2_t)(b) ) >> FP_DP;
}
fxp_t fixedDiv( fxp_t a, fxp_t b ){
    return ( (fxp2_t)(a) << FP_DP ) / (fxp2_t)(b);
}
#endif