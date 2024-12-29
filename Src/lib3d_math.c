#include "../Inc/lib3d_math.h"

#ifdef LIB3D_USE_FIXED_POINT_ARITHMETIC
fxp_t floatToFixed( flp_t num ){
    return (fxp_t)( num * (flp_t)( 1 << FP_DP ) + ( num >= 0 ? 0.5 : -0.5 ) );
}
flp_t fixedToFloat( fxp_t num ){
    return (flp_t)(num) / (flp_t)( 1 << FP_DP );
}
fxp_t fixedMul( fxp_t a, fxp_t b ){
    return ( (fxp2_t)(a) * (fxp2_t)(b) ) >> FP_DP;
}
fxp_t fixedDiv( fxp_t a, fxp_t b ){
    return ( (fxp2_t)(a) << FP_DP ) / (fxp2_t)(b);
}
#endif

rtnl_t getZeroRtnl(void){
#ifdef LIB3D_USE_FIXED_POINT_ARITHMETIC
    return floatToFixed(0.0f);
#else
    return 0.0f;
#endif
}

rot_t getZeroRot(void){
    rot_t r;
#ifdef LIB3D_USE_FIXED_POINT_ARITHMETIC
    r.yaw = floatToFixed(0.0f);
    r.pitch = floatToFixed(0.0f);
    r.roll = floatToFixed(0.0f);
#else
    r.yaw = 0.0f;
    r.pitch = 0.0f;
    r.roll = 0.0f;
#endif
    return r;
}

vec4_t getZeroVec(void){
    vec4_t v;
#ifdef LIB3D_USE_FIXED_POINT_ARITHMETIC
    v.x = floatToFixed(0.0f);
    v.y = floatToFixed(0.0f);
    v.z = floatToFixed(0.0f);
    v.h = floatToFixed(1.0f);
#else
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    v.h = 1.0f;
#endif
    return v;
}
