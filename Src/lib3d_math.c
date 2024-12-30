#include "../Inc/lib3d_math.h"

#ifdef LIB3D_USE_FIXED_POINT_ARITHMETIC
l3d_fxp_t l3d_floatToFixed( l3d_flp_t num ){
    return (l3d_fxp_t)( num * (l3d_flp_t)( 1 << L3D_FP_DP ) + ( num >= 0 ? 0.5 : -0.5 ) );
}
l3d_flp_t l3d_fixedToFloat( l3d_fxp_t num ){
    return (l3d_flp_t)(num) / (l3d_flp_t)( 1 << L3D_FP_DP );
}
l3d_fxp_t l3d_fixedMul( l3d_fxp_t a, l3d_fxp_t b ){
    return ( (l3d_fxp2_t)(a) * (l3d_fxp2_t)(b) ) >> L3D_FP_DP;
}
l3d_fxp_t l3d_fixedDiv( l3d_fxp_t a, l3d_fxp_t b ){
    return ( (l3d_fxp2_t)(a) << L3D_FP_DP ) / (l3d_fxp2_t)(b);
}
#endif

l3d_rtnl_t l3d_getZeroRtnl(void){
#ifdef LIB3D_USE_FIXED_POINT_ARITHMETIC
    return l3d_floatToFixed(0.0f);
#else
    return 0.0f;
#endif
}

l3d_rot_t l3d_getZeroRot(void){
    l3d_rot_t r;
#ifdef LIB3D_USE_FIXED_POINT_ARITHMETIC
    r.yaw = l3d_floatToFixed(0.0f);
    r.pitch = l3d_floatToFixed(0.0f);
    r.roll = l3d_floatToFixed(0.0f);
#else
    r.yaw = 0.0f;
    r.pitch = 0.0f;
    r.roll = 0.0f;
#endif
    return r;
}

l3d_vec4_t l3d_getZeroVec(void){
    l3d_vec4_t v;
#ifdef LIB3D_USE_FIXED_POINT_ARITHMETIC
    v.x = l3d_floatToFixed(0.0f);
    v.y = l3d_floatToFixed(0.0f);
    v.z = l3d_floatToFixed(0.0f);
    v.h = l3d_floatToFixed(1.0f);
#else
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    v.h = 1.0f;
#endif
    return v;
}
