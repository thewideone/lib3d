#include "../Inc/lib3d_math.h"

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
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

// 
// Returns signed int32 value.
// If fixed point arithmetic is used, the argument is converted to l3d_flp_t.
// 
int32_t l3d_rationalToInt32(l3d_rtnl_t num){
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return (int32_t)l3d_fixedToFloat(num);
#else
    return (int32_t)num;
#endif
}

// 
// Returns a rational number.
// If fixed point arithmetic is used, the argument is converted to l3d_fxp_t.
// 
l3d_rtnl_t l3d_floatToRational(l3d_flp_t num){
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return l3d_floatToFixed(num);
#else
    return num;
#endif
}

// 
// Returns rotation struct.
// If fixed point arithmetic is used, arguments are converted to l3d_fxp_t.
// 
l3d_rot_t l3d_getRotFromFloat(l3d_flp_t yaw, l3d_flp_t pitch, l3d_flp_t roll){
    l3d_rot_t r;
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    r.yaw = l3d_floatToFixed(yaw);
    r.pitch = l3d_floatToFixed(pitch);
    r.roll = l3d_floatToFixed(roll);
#else
    r.yaw = yaw;
    r.pitch = pitch;
    r.roll = roll;
#endif
    return r;
}

// 
// Returns vec4.
// If fixed point arithmetic is used, arguments are converted to l3d_fxp_t.
// 
l3d_vec4_t l3d_getVec4FromFloat(l3d_flp_t x, l3d_flp_t y, l3d_flp_t z, l3d_flp_t h){
    l3d_vec4_t v;
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    v.x = l3d_floatToFixed(x);
    v.y = l3d_floatToFixed(y);
    v.z = l3d_floatToFixed(z);
    v.h = l3d_floatToFixed(h);
#else
    v.x = x;
    v.y = y;
    v.z = z;
    v.h = h;
#endif
    return v;
}

l3d_rtnl_t l3d_getZeroRtnl(void){
    return l3d_floatToRational(0.0f);
}

l3d_rot_t l3d_getZeroRot(void){
    return l3d_getRotFromFloat(0.0f, 0.0f, 0.0f);
}

l3d_vec4_t l3d_getZeroVec4(void){
    return l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);
}
