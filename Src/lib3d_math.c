#include "../Inc/lib3d_math.h"
#include "../Inc/lib3d_core.h" // for l3d_errorHandler()
#include "../Inc/lib3d_util.h"  // for debug print
#include "math.h"
#include <string.h> // for memset()
#include <stdio.h> // for sprintf()
#include <math.h>   // for M_PI

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

l3d_flp_t l3d_rationalToFloat(l3d_rtnl_t num){
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return l3d_fixedToFloat(num);
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

l3d_quat_t l3d_getQuatFromFloat(l3d_flp_t w, l3d_flp_t x, l3d_flp_t y, l3d_flp_t z) {
    return (l3d_quat_t){l3d_floatToRational(w), l3d_floatToRational(x), l3d_floatToRational(y), l3d_floatToRational(z)};
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

l3d_quat_t l3d_getIdentityQuat(void) {
    return l3d_getQuatFromFloat(1.0f, 0.0f, 0.0f, 0.0f);
}

l3d_rtnl_t l3d_degToRad(l3d_rtnl_t deg) {
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return l3d_fixedMul( deg, l3d_floatToFixed(L3D_PI / 180.0f) );
#else
    return deg * (L3D_PI / 180.0f);
#endif
}

l3d_rtnl_t l3d_radToDeg(l3d_rtnl_t rad) {
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return l3d_fixedMul( rad, l3d_floatToFixed(180.0f / L3D_PI) );
#else
    return rad * (180.0f / L3D_PI);
#endif
}

l3d_rot_t l3d_quatToEuler(const l3d_quat_t *q) {
    // l3d_rot_t result;
    l3d_rot_t euler;

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    // From
    // https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html
    l3d_flp_t qw = l3d_fixedToFloat(q->w);
    l3d_flp_t qx = l3d_fixedToFloat(q->x);
    l3d_flp_t qy = l3d_fixedToFloat(q->y);
    l3d_flp_t qz = l3d_fixedToFloat(q->z);

    l3d_flp_t yaw, pitch, roll;

    yaw = atan2f(2.0f * (qw*qz + qx*qy), qw*qw + qx*qx - qy*qy - qz*qz);
    roll = asinf(2.0f * (qw*qy - qx*qz));

    if (roll - M_PI/2.0f < L3D_EPSILON_FLP) {
        pitch = 0.0f;
        yaw = -2.0f * atan2f(qx, qw);
    }
    else if (roll - (-M_PI/2.0f) < L3D_EPSILON_FLP) {
        pitch = 0.0f;
        yaw = 2.0f * atan2f(qx, qw);
    }
    else
        pitch = atan2f(2.0f * (qw*qx + qy*qz), qw*qw - qx*qx - qy*qy + qz*qz);

    euler.yaw = l3d_floatToFixed(yaw);
    euler.pitch = l3d_floatToFixed(pitch);
    euler.roll = l3d_floatToFixed(roll);

    // First test if the formula even works
    // l3d_rtnl_t qwx = l3d_fixedMul(q->w, q->x);
    // l3d_rtnl_t qyz = l3d_fixedMul(q->y, q->z);
    // l3d_rtnl_t qx2 = l3d_fixedMul(q->x, q->x);
    // l3d_rtnl_t qy2 = l3d_fixedMul(q->y, q->y);
    // l3d_rtnl_t qz2 = l3d_fixedMul(q->z, q->z);
    // l3d_rtnl_t qwy = l3d_fixedMul(q->w, q->y);
    // l3d_rtnl_t qxz = l3d_fixedMul(q->x, q->z);
    // l3d_rtnl_t qwz = l3d_fixedMul(q->w, q->z);
    // l3d_rtnl_t qxy = l3d_fixedMul(q->x, q->y);

    // result.yaw = l3d_floatToRational(atan2f(l3d_fixedMul(2, (qwx + qyz)), 1-l3d_fixedMul(2, (qx2 + qy2))));
    // result.pitch = l3d_floatToRational(asinf(2*(qwy + qxz)));
    // result.roll = l3d_floatToRational(atan2f(2*(qwz + qxy), 1-2*(qy2 + qz2)));

    // l3d_flp_t w, x, y, z;
    // w = l3d_fixedToFloat(q->w);
    // x = l3d_fixedToFloat(q->x);
    // y = l3d_fixedToFloat(q->y);
    // z = l3d_fixedToFloat(q->z);
    // l3d_flp_t yaw, pitch, roll;
    // yaw = atan2f(2.0f*(w * x + y * z), 1.0f-2.0f*(x * x + y * y));
    // pitch = asinf(2.0f*(w * y + x * z));
    // roll = atan2f(2.0f*(w * z + x * y), 1.0f-2.0f*(y * y + z * z));

    // result.yaw = l3d_floatToFixed(yaw);
    // result.pitch = l3d_floatToFixed(pitch);
    // result.roll = l3d_floatToFixed(roll);

    // // CHATGPT 3
    // l3d_flp_t qw = l3d_fixedToFloat(q->w);
    // l3d_flp_t qx = l3d_fixedToFloat(q->x);
    // l3d_flp_t qy = l3d_fixedToFloat(q->y);
    // l3d_flp_t qz = l3d_fixedToFloat(q->z);

    // // Pitch (X-axis rotation)
    // l3d_flp_t sinp = 2.0f * (qw * qx - qy * qz);
    // if (fabsf(sinp) >= 1.0f)
    //     euler.pitch = l3d_floatToFixed(copysignf(M_PI / 2.0f, sinp)); // Use 90 degrees if out of range
    // else
    //     euler.pitch = l3d_floatToFixed(asinf(sinp));

    // // Yaw (Z-axis)
    // euler.yaw = l3d_floatToFixed(atan2f(
    //     2.0f * (qw * qz + qx * qy),
    //     1.0f - 2.0f * (qx * qx + qz * qz)
    // ));

    // // Roll (Y-axis)
    // euler.roll = l3d_floatToFixed(atan2f(
    //     2.0f * (qw * qy + qz * qx),
    //     1.0f - 2.0f * (qx * qx + qy * qy)
    // ));

    // CHATGPT 2
    // l3d_flp_t x = l3d_fixedToFloat(q->x);
    // l3d_flp_t y = l3d_fixedToFloat(q->y);
    // l3d_flp_t z = l3d_fixedToFloat(q->z);
    // l3d_flp_t w = l3d_fixedToFloat(q->w);

    // // ZXY extraction
    // l3d_flp_t sin_pitch = 2.0f * (w * x - y * z);
    // euler.pitch = fabsf(sin_pitch) >= 1.0f
    //     ? l3d_floatToFixed(copysignf(M_PI / 2.0f, sin_pitch))
    //     : l3d_floatToFixed(asinf(sin_pitch));

    // euler.yaw = l3d_floatToFixed(atan2f(
    //     2.0f * (x * y + w * z),
    //     w * w + x * x - y * y - z * z
    // ));

    // euler.roll = l3d_floatToFixed(atan2f(
    //     2.0f * (x * z + w * y),
    //     w * w - x * x - y * y + z * z
    // ));

#else
    // result.yaw = atan2f(2.0f*(q->w * q->x + q->y * q->z), 1.0f-2.0f*(q->x * q->x + q->y * q->y));
    // result.pitch = asinf(2.0f*(q->w * q->y + q->x * q->z));
    // result.roll = atan2f(2.0f*(q->w * q->z + q->x * q->y), 1.0f-2.0f*(q->y * q->y + q->z * q->z));

    // Pitch (X-axis rotation)
    float sinp = 2.0f * (q->w * q->x - q->y * q->z);
    if (fabsf(sinp) >= 1.0f)
        euler.pitch = copysignf(M_PI / 2.0f, sinp); // Use 90 degrees if out of range
    else
        euler.pitch = asinf(sinp);

    // Yaw (Z-axis)
    euler.yaw = atan2f(
        2.0f * (q->w * q->z + q->x * q->y),
        1.0f - 2.0f * (q->x * q->x + q->z * q->z)
    );

    // Roll (Y-axis)
    euler.roll = atan2f(
        2.0f * (q->w * q->y + q->z * q->x),
        1.0f - 2.0f * (q->x * q->x + q->y * q->y)
    );

#endif
    // return result;
    return euler;
}

l3d_quat_t l3d_eulerToQuat(const l3d_rot_t *r) {
    // First test if the formula even forks
    l3d_flp_t half_yaw = l3d_rationalToFloat(r->yaw) * 0.5f;
    l3d_flp_t half_pitch = l3d_rationalToFloat(r->pitch) * 0.5f;
    l3d_flp_t half_roll = l3d_rationalToFloat(r->roll) * 0.5f;

    // l3d_flp_t cy = cosf(half_yaw);
    // l3d_flp_t sy = sinf(half_yaw);
    // l3d_flp_t cp = cosf(half_pitch);
    // l3d_flp_t sp = sinf(half_pitch);
    // l3d_flp_t cr = cosf(half_roll);
    // l3d_flp_t sr = sinf(half_roll);

    l3d_quat_t result;
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    // result.w = l3d_floatToFixed(cy * cp * cr + sy * sp * sr);
    // result.x = l3d_floatToFixed(sy * cp * cr - cy * sp * sr);
    // result.y = l3d_floatToFixed(cy * sp * cr + sy * cp * sr);
    // result.z = l3d_floatToFixed(cy * cp * sr - sy * sp * cr);

    l3d_flp_t cz = cosf(half_yaw);
    l3d_flp_t sz = sinf(half_yaw);
    l3d_flp_t cx = cosf(half_pitch);
    l3d_flp_t sx = sinf(half_pitch);
    l3d_flp_t cy = cosf(half_roll);
    l3d_flp_t sy = sinf(half_roll);

    // Rotation order: Z (yaw), then X (pitch), then Y (roll)
    result.w = l3d_floatToFixed(cz * cx * cy - sz * sx * sy);
    result.x = l3d_floatToFixed(cz * sx * cy - sz * cx * sy);
    result.y = l3d_floatToFixed(cz * cx * sy + sz * sx * cy);
    result.z = l3d_floatToFixed(sz * cx * cy + cz * sx * sy);

    // // Rotation order: Z (yaw), then X (pitch), then Y (roll)
    // result.w = cz * cx * cy - sz * sx * sy;
    // result.x = cz * sx * cy - sz * cx * sy;
    // result.y = cz * cx * sy + sz * sx * cy;
    // result.z = sz * cx * cy + cz * sx * sy;

#else
    l3d_flp_t cz = cosf(half_yaw);
    l3d_flp_t sz = sinf(half_yaw);
    l3d_flp_t cx = cosf(half_pitch);
    l3d_flp_t sx = sinf(half_pitch);
    l3d_flp_t cy = cosf(half_roll);
    l3d_flp_t sy = sinf(half_roll);
    // Rotation order: Z (yaw), then X (pitch), then Y (roll)
    result.w = (cz * cx * cy - sz * sx * sy);
    result.x = (cz * sx * cy - sz * cx * sy);
    result.y = (cz * cx * sy + sz * sx * cy);
    result.z = (sz * cx * cy + cz * sx * sy);
#endif

    return result;
}

void l3d_quatToRotMat(l3d_mat4x4_t *m, const l3d_quat_t *q) {
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    // Equation (7b) from https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html
    
    l3d_flp_t qw = l3d_fixedToFloat(q->w);
    l3d_flp_t qx = l3d_fixedToFloat(q->x);
    l3d_flp_t qy = l3d_fixedToFloat(q->y);
    l3d_flp_t qz = l3d_fixedToFloat(q->z);

    m->m[0][0] = l3d_floatToRational(1.0f - 2.0f * (qy*qy + qz*qz));
    m->m[0][1] = l3d_floatToRational(2.0f * qx*qy - 2.0f * qw*qz);
    m->m[0][2] = l3d_floatToRational(2.0f * qx*qz + 2.0f * qw*qy);
    m->m[0][3] = l3d_floatToRational(0.0f);

    m->m[1][0] = l3d_floatToRational(2.0f * qx*qy + 2.0f * qw * qz);
    m->m[1][1] = l3d_floatToRational(1.0f - 2.0f * qx * qx - 2.0f * qz * qz);
    m->m[1][2] = l3d_floatToRational(2.0f * qy * qz - 2.0f * qw * qx);
    m->m[1][3] = l3d_floatToRational(0.0f);

    m->m[2][0] = l3d_floatToRational(2.0f * qx * qz - 2.0f * qw * qy);
    m->m[2][1] = l3d_floatToRational(2.0f * qy * qz + 2.0f * qw * qx);
    m->m[2][2] = l3d_floatToRational(1.0f - 2.0f * qx * qx - 2.0f * qy * qy);
    m->m[2][3] = l3d_floatToRational(0.0f);

    m->m[3][0] = l3d_floatToRational(0.0f);
    m->m[3][1] = l3d_floatToRational(0.0f);
    m->m[3][2] = l3d_floatToRational(0.0f);
    m->m[3][3] = l3d_floatToRational(1.0f);
#else
    // Equation (7b) from https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html
    m->m[0][0] = 1.0f - 2.0f * (q->y*q->y + q->z*q->z);
    m->m[0][1] = 2.0f * q->x*q->y - 2.0f * q->w*q->z;
    m->m[0][2] = 2.0f * q->x*q->z + 2.0f * q->w*q->y;
    m->m[0][3] = 0.0f;

    m->m[1][0] = 2.0f * q->x*q->y + 2.0f * q->w * q->z;
    m->m[1][1] = 1.0f - 2.0f * q->x * q->x - 2.0f * q->z * q->z;
    m->m[1][2] = 2.0f * q->y * q->z - 2.0f * q->w * q->x;
    m->m[1][3] = 0.0f;

    m->m[2][0] = 2.0f * q->x * q->z - 2.0f * q->w * q->y;
    m->m[2][1] = 2.0f * q->y * q->z + 2.0f * q->w * q->x;
    m->m[2][2] = 1.0f - 2.0f * q->x * q->x - 2.0f * q->y * q->y;
    m->m[2][3] = 0.0f;

    m->m[3][0] = 0.0f;
    m->m[3][1] = 0.0f;
    m->m[3][2] = 0.0f;
    m->m[3][3] = 1.0f;
#endif // L3D_USE_FIXED_POINT_ARITHMETIC
}

l3d_quat_t l3d_axisAngleToQuat(const l3d_vec4_t *axis, l3d_rtnl_t angle_rad) {
    // Eq. 4ab-e from
    // https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html
    l3d_quat_t q;
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    l3d_flp_t half_angle_flp = l3d_fixedToFloat(angle_rad) / 2.0f;
    q.w = l3d_floatToFixed(cosf(half_angle_flp));
    q.x = l3d_fixedMul(axis->x, l3d_floatToFixed(sinf(half_angle_flp)));
    q.y = l3d_fixedMul(axis->y, l3d_floatToFixed(sinf(half_angle_flp)));
    q.z = l3d_fixedMul(axis->z, l3d_floatToFixed(sinf(half_angle_flp)));
#else
    q.w = cosf(angle_rad / 2.0f);
    q.x = axis->x * sinf(angle_rad / 2.0f);
    q.y = axis->y * sinf(angle_rad / 2.0f);
    q.z = axis->z * sinf(angle_rad / 2.0f);
#endif // L3D_USE_FIXED_POINT_ARITHMETIC
    return q;
}

l3d_vec4_t l3d_rotateVecByQuat(const l3d_vec4_t *v, const l3d_quat_t *q) {
    // Active rotation (point is rotated with respect to the coordinate system)
    // p' = q^(-1) * p * q
    l3d_quat_t p = {l3d_floatToRational(0.0f), v->x, v->y, v->z};
    l3d_quat_t q_inv = l3d_quat_inverse(q);

    p = l3d_quat_mul(&q_inv, &p);
    p = l3d_quat_mul(&p, q);

    return (l3d_vec4_t){p.x, p.y, p.z, l3d_floatToRational(1.0f)};
}

l3d_vec4_t l3d_vec4_add( const l3d_vec4_t *v1, const l3d_vec4_t *v2 ){
    return (l3d_vec4_t){ v1->x + v2->x, v1->y + v2->y, v1->z + v2->z, l3d_floatToRational(1.0f) };
}

l3d_vec4_t l3d_vec4_sub( const l3d_vec4_t *v1, const l3d_vec4_t *v2 ){
    return (l3d_vec4_t){ v1->x - v2->x, v1->y - v2->y, v1->z - v2->z, l3d_floatToRational(1.0f) };
}

l3d_vec4_t l3d_vec4_mul( const l3d_vec4_t *v, l3d_rtnl_t k ){
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return (l3d_vec4_t){ l3d_fixedMul(v->x, k), l3d_fixedMul(v->y, k), l3d_fixedMul(v->z, k), v->h };
#else
    return (l3d_vec4_t){ v->x * k, v->y * k, v->z * k, v->h };
#endif
}

l3d_vec4_t l3d_vec4_div( const l3d_vec4_t *v, l3d_rtnl_t k ){
    if( k == l3d_floatToRational(0.0f) ){    // or less than some threshold???
        L3D_DEBUG_PRINT( "Error: division by 0. Aborting.\n" );
        // exit(0);    // maybe replace with something better
        l3d_errorHandler();
    }
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return (l3d_vec4_t){ l3d_fixedDiv(v->x, k), l3d_fixedDiv(v->y, k), l3d_fixedDiv(v->z, k), v->h };
#else
    return (l3d_vec4_t){ v->x / k, v->y / k, v->z / k, v->h };
#endif
}

l3d_rtnl_t l3d_vec4_dotProduct( const l3d_vec4_t *v1, const l3d_vec4_t *v2 ){
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return l3d_fixedMul(v1->x, v2->x) + l3d_fixedMul( v1->y, v2->y ) + l3d_fixedMul( v1->z, v2->z );
#else
    return (v1->x)*(v2->x) + (v1->y)*(v2->y) + (v1->z)*(v2->z);
#endif
}

l3d_vec4_t l3d_vec4_crossProduct( const l3d_vec4_t *v1, const l3d_vec4_t *v2 ){
    static l3d_vec4_t v;    // why static?
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    v.x = l3d_fixedMul( v1->y, v2->z ) - l3d_fixedMul( v1->z, v2->y );
	v.y = l3d_fixedMul( v1->z, v2->x ) - l3d_fixedMul( v1->x, v2->z );
	v.z = l3d_fixedMul( v1->x, v2->y ) - l3d_fixedMul( v1->y, v2->x );
    v.h = l3d_floatToFixed(1.0f);
#else
	v.x = (v1->y) * (v2->z) - (v1->z) * (v2->y);
	v.y = (v1->z) * (v2->x) - (v1->x) * (v2->z);
	v.z = (v1->x) * (v2->y) - (v1->y) * (v2->x);
    v.h = 1.0f;
#endif
	return v;
}

l3d_rtnl_t l3d_vec4_length( const l3d_vec4_t *v ){
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return l3d_floatToFixed( sqrt( l3d_fixedToFloat( l3d_vec4_dotProduct(v, v) ) ) );
#else
    return sqrtf( l3d_vec4_dotProduct(v, v) );
#endif
}

bool l3d_vec4_isNormalised( const l3d_vec4_t *v ) {
    return (l3d_vec4_length( v ) - l3d_floatToRational(1.0f) < L3D_EPSILON_RTNL);
}

l3d_vec4_t l3d_vec4_normalise( const l3d_vec4_t *v ){
    l3d_rtnl_t l = l3d_vec4_length( v );
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC 
    if( l == l3d_floatToFixed( 0.0f ) )
        l = l3d_floatToFixed( 0.0001f );
#endif
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return (l3d_vec4_t){ l3d_fixedDiv( v->x, l ), l3d_fixedDiv( v->y, l), l3d_fixedDiv( v->z, l ), v->h };
#else
    return (l3d_vec4_t){ v->x / l, v->y / l, v->z / l, v->h };
#endif
}

// 
// Return negated value of given vector,
// that is -v
// 
l3d_vec4_t l3d_vec4_negate( const l3d_vec4_t *v ) {
    return (l3d_vec4_t){-v->x, -v->y, -v->z, v->h};
}

l3d_quat_t l3d_quat_add(const l3d_quat_t *q1, const l3d_quat_t *q2) {
    return (l3d_quat_t){q1->w + q2->w, q1->x + q2->x, q1->y + q2->y, q1->z + q2->z};
}

l3d_quat_t l3d_quat_mul(const l3d_quat_t *q1, const l3d_quat_t *q2) {
    l3d_quat_t result;
    l3d_vec4_t v1 = {q1->x, q1->y, q1->z};
    l3d_vec4_t v2 = {q2->x, q2->y, q2->z};

    // Scalar part:
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    result.w = l3d_fixedMul(q1->w, q2->w) - l3d_vec4_dotProduct(&v1, &v2);
#else
    result.w = q1->w * q2->w - l3d_vec4_dotProduct(&v1, &v2);
#endif

    // Vector part:
    // v_result = q1 x q2 + q1->w * v2 + q2->w * v1
    l3d_vec4_t cross = l3d_vec4_crossProduct(&v1, &v2);
    l3d_vec4_t v_result = l3d_vec4_mul(&v2, q1->w);
    v2 = l3d_vec4_mul(&v1, q2->w);
    v1 = l3d_vec4_add(&v_result, &v2);
    v_result = l3d_vec4_add(&cross, &v1);

    result.x = v_result.x;
    result.y = v_result.y;
    result.z = v_result.z;
    
    return result;
}

l3d_rtnl_t l3d_quat_norm(const l3d_quat_t *q) {
    // Norm |q| = sqrt(q.w^2 + q.x^2 + q.y^2 + q.z^2)
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    l3d_rtnl_t norm = l3d_fixedMul(q->w, q->w) + l3d_fixedMul(q->x, q->x) + l3d_fixedMul(q->y, q->y) + l3d_fixedMul(q->z, q->z);
    norm = l3d_floatToFixed(sqrtf(l3d_fixedToFloat(norm)));
#else
    l3d_rtnl_t norm = sqrtf((q->w * q->w) + (q->x * q->x) + (q->y * q->y) + (q->z * q->z));
#endif
    return norm;
}

l3d_quat_t l3d_quat_normalise(const l3d_quat_t *q) {
    // // Norm |q| = sqrt(q*q), where q* is complex conjugate of q
    // l3d_quat_t conj = l3d_quat_complexConjugate(q);
    // l3d_quat_t product = l3d_quat_mul(q, &conj);
    // L3D_DEBUG_PRINT_QUAT(product);
    // l3d_rtnl_t norm = l3d_floatToRational(sqrtf( l3d_rationalToFloat(product.w) ));
    // L3D_DEBUG_PRINT_RTNL(norm);

    // Or simply
    l3d_rtnl_t norm = l3d_quat_norm(q);

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return (l3d_quat_t){ l3d_fixedDiv( q->w, norm ), l3d_fixedDiv( q->x, norm ), l3d_fixedDiv( q->y, norm), l3d_fixedDiv( q->z, norm ) };
#else
    return (l3d_quat_t){ q->w / norm, q->x / norm, q->y / norm, q->z / norm };
#endif
}

l3d_quat_t l3d_quat_complexConjugate(const l3d_quat_t *q) {
    return (l3d_quat_t){q->w, -q->x, -q->y, -q->z};
}

l3d_quat_t l3d_quat_inverse(const l3d_quat_t *q) {
    // q^(-1) = q* / |q|^2
    // Unnumbered equation from section 2.2 of
    // https://graphics.stanford.edu/courses/cs348a-17-winter/Papers/quaternion.pdf
    l3d_quat_t conj = l3d_quat_complexConjugate(q);
    l3d_rtnl_t norm2 = l3d_quat_norm(q);    // squared norm

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    norm2 = l3d_fixedMul(norm2, norm2);
    return (l3d_quat_t){ l3d_fixedDiv( conj.w, norm2 ), l3d_fixedDiv( conj.x, norm2 ), l3d_fixedDiv( conj.y, norm2), l3d_fixedDiv( conj.z, norm2 ) };
#else
    norm2 = norm2 * norm2;
    return (l3d_quat_t){ conj.w / norm2, conj.x / norm2, conj.y / norm2, conj.z / norm2 };
#endif
}

// 
// Add two matrices together
// m_out - output matrix
// m1 - first input matrix
// m2 - second input matrix
// 
void l3d_mat4x4_addMatrix( l3d_mat4x4_t *m_out, const l3d_mat4x4_t *m1, const l3d_mat4x4_t *m2 ) {
    for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			m_out->m[r][c] = m1->m[r][c] + m2->m[r][c];
}

// 
// Multiply matrix by a constant
// m_out - output matrix
// m_in - input matrix
// k - constant
// 
void l3d_mat4x4_mulConst( l3d_mat4x4_t *m_out, l3d_mat4x4_t *m_in, l3d_rtnl_t k ) {
    for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
            m_out->m[r][c] = l3d_fixedMul( m_in->m[r][c], k );
#else
			m_out->m[r][c] = m_in->m[r][c] * k;
#endif
}

l3d_vec4_t l3d_mat4x4_mulVec4( const l3d_mat4x4_t *m, const l3d_vec4_t *v ){
    l3d_vec4_t o;
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    o.x = l3d_fixedMul( v->x, m->m[0][0] ) + l3d_fixedMul( v->y, m->m[1][0] ) + l3d_fixedMul( v->z, m->m[2][0] ) + l3d_fixedMul( v->h, m->m[3][0] );
    o.y = l3d_fixedMul( v->x, m->m[0][1] ) + l3d_fixedMul( v->y, m->m[1][1] ) + l3d_fixedMul( v->z, m->m[2][1] ) + l3d_fixedMul( v->h, m->m[3][1] );
    o.z = l3d_fixedMul( v->x, m->m[0][2] ) + l3d_fixedMul( v->y, m->m[1][2] ) + l3d_fixedMul( v->z, m->m[2][2] ) + l3d_fixedMul( v->h, m->m[3][2] );
    o.h = l3d_fixedMul( v->x, m->m[0][3] ) + l3d_fixedMul( v->y, m->m[1][3] ) + l3d_fixedMul( v->z, m->m[2][3] ) + l3d_fixedMul( v->h, m->m[3][3] );
#else
	o.x = v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0] + v->h * m->m[3][0];
	o.y = v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1] + v->h * m->m[3][1];
	o.z = v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2] + v->h * m->m[3][2];
    o.h = v->x * m->m[0][3] + v->y * m->m[1][3] + v->z * m->m[2][3] + v->h * m->m[3][3];
#endif
	return o;
}

void l3d_mat4x4_makeEmpty( l3d_mat4x4_t *m ){
    memset( m->m, l3d_floatToRational(0.0f), sizeof( m->m ) );
}

void l3d_mat4x4_makeIdentity( l3d_mat4x4_t *m ){
    l3d_mat4x4_makeEmpty( m );

    m->m[0][0] = l3d_floatToRational(1.0f);
    m->m[1][1] = l3d_floatToRational(1.0f);
    m->m[2][2] = l3d_floatToRational(1.0f);
    m->m[3][3] = l3d_floatToRational(1.0f);
}

void l3d_mat4x4_makeRotZ( l3d_mat4x4_t *m, l3d_rtnl_t angle_rad ){
	l3d_mat4x4_makeEmpty( m );
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    m->m[0][0] = l3d_floatToFixed( cosf( l3d_fixedToFloat( angle_rad ) ) );
    m->m[0][1] = l3d_floatToFixed( -sinf( l3d_fixedToFloat( angle_rad ) ) );
    m->m[1][0] = l3d_floatToFixed( sinf( l3d_fixedToFloat( angle_rad ) ) );
    m->m[1][1] = l3d_floatToFixed( cosf( l3d_fixedToFloat( angle_rad ) ) );
    m->m[2][2] = l3d_floatToFixed( 1.0f );
    m->m[3][3] = l3d_floatToFixed( 1.0f );
#else
    m->m[0][0] = cosf( angle_rad );
    m->m[0][1] = sinf( angle_rad );
    m->m[1][0] = -sinf( angle_rad );
    m->m[1][1] = cosf( angle_rad );
    m->m[2][2] = 1.0f;
    m->m[3][3] = 1.0f;
#endif
}

void l3d_mat4x4_makeRotX( l3d_mat4x4_t *m, l3d_rtnl_t angle_rad ){
	l3d_mat4x4_makeEmpty( m );
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    m->m[0][0] = l3d_floatToFixed( 1.0f );
    m->m[1][1] = l3d_floatToFixed( cosf( l3d_fixedToFloat( angle_rad ) ) );
    m->m[1][2] = l3d_floatToFixed( -sinf( l3d_fixedToFloat( angle_rad ) ) );
    m->m[2][1] = l3d_floatToFixed( sinf( l3d_fixedToFloat( angle_rad ) ) );
    m->m[2][2] = l3d_floatToFixed( cosf( l3d_fixedToFloat( angle_rad ) ) );
    m->m[3][3] = l3d_floatToFixed( 1.0f );
#else
    m->m[0][0] = 1.0f;
    m->m[1][1] = cosf( angle_rad );
    m->m[1][2] = sinf( angle_rad );
    m->m[2][1] = -sinf( angle_rad );
    m->m[2][2] = cosf( angle_rad );
    m->m[3][3] = 1.0f;
#endif
}

#ifdef L3D_CAMERA_MOVABLE
void l3d_mat4x4_makeRotY( l3d_mat4x4_t *m, l3d_rtnl_t angle_rad){
    l3d_mat4x4_makeEmpty( m );
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    m->m[0][0] = l3d_floatToFixed( cosf( l3d_fixedToFloat( angle_rad ) ) );
	m->m[0][2] = l3d_floatToFixed( sinf( l3d_fixedToFloat( angle_rad ) ) );
	m->m[2][0] = l3d_floatToFixed( -sinf( l3d_fixedToFloat( angle_rad ) ) );
	m->m[1][1] = l3d_floatToFixed( 1.0f );
	m->m[2][2] = l3d_floatToFixed( cosf( l3d_fixedToFloat( angle_rad ) ) );
	m->m[3][3] = l3d_floatToFixed( 1.0f );
#else
	m->m[0][0] = cosf(angle_rad);
	m->m[0][2] = sinf(angle_rad);
	m->m[2][0] = -sinf(angle_rad);
	m->m[1][1] = 1.0f;
	m->m[2][2] = cosf(angle_rad);
	m->m[3][3] = 1.0f;
#endif
}
#endif // L3D_CAMERA_MOVABLE

// 
// DOES NOT WORK YET
// 
// Make a matrix representing rotation
// along axis defined by the unit vector n,
// and a point p located along this axis,
// about an angle angle_rad.
// 
// This function represents transformation
// shown in equation (11) at https://arxiv.org/pdf/1404.6055
// 
// m - output matrix
// 
// n = (a,b,c)
// p = (x0, y0, z0)
// 
l3d_err_t l3d_mat4x4_makeRotGeneral( l3d_mat4x4_t *m, const l3d_vec4_t *n, const l3d_vec4_t *p, l3d_rtnl_t angle_rad ) {
    // If n is not a unit vector (length > 1.0), abort
    if ((l3d_vec4_length(n) - l3d_floatToRational(1.0f)) > L3D_EPSILON_RTNL)
        return L3D_WRONG_PARAM;
    
    l3d_mat4x4_makeEmpty(m);
    l3d_mat4x4_t m_i, m_a, m_t; // identity, antisymmetric, and translation (different than the result of l3d_mat4x4_makeTranslation()) matrices
    l3d_mat4x4_makeIdentity(&m_i);
    l3d_mat4x4_makeEmpty(&m_a);
    l3d_mat4x4_makeIdentity(&m_t);

    // Components of the normal vector
    l3d_rtnl_t a = n->x;
    l3d_rtnl_t b = n->y;
    l3d_rtnl_t c = n->z;
    // Components of the point
    l3d_rtnl_t x0 = p->x;
    l3d_rtnl_t y0 = p->y;
    l3d_rtnl_t z0 = p->z;

    // m_a.m[0][0] = ;
    m_a.m[0][1] = -c;
    m_a.m[0][2] = b;
    // m_a.m[0][3] = ;

    m_a.m[1][0] = c;
    // m_a.m[1][1] = ;
    m_a.m[1][2] = -a;
    // m_a.m[1][3] = ;

    m_a.m[2][0] = -b;
    m_a.m[2][1] = a;
    // m_a.m[2][2] = ;
    // m_a.m[2][3] = ;


    // m_a.m[0][0] = ;
    m_a.m[0][1] = -c;
    m_a.m[0][2] = b;
    // m_a.m[0][3] = ;

    m_a.m[1][0] = c;
    // m_a.m[1][1] = ;
    m_a.m[1][2] = -a;
    // m_a.m[1][3] = ;

    m_a.m[2][0] = -b;
    m_a.m[2][1] = a;
    // m_a.m[2][2] = ;
    // m_a.m[2][3] = ;


    m_t.m[0][3] = -x0;
    m_t.m[1][3] = -y0;
    m_t.m[2][3] = -z0;

    // make m_i a central dilitation matrix:
    // m_i.m[3][3] = l3d_floatToRational( 2.0f - cosf( l3d_rationalToFloat(angle_rad) ) );

    // m = m_i + ( sin(angle_rad)*m_a + (1-cos(angle_rad))*m_a^2 ) * m_t

    l3d_mat4x4_t m_tmp1, m_tmp2;
    l3d_mat4x4_mulConst(&m_tmp1, &m_a, l3d_floatToRational( sinf( l3d_rationalToFloat(angle_rad) ) ));  // m_tmp1 = sin(angle_rad) * m_a

    l3d_mat4x4_mulMatrix(&m_tmp2, &m_a, &m_a);          // m_tmp2 = m_a^2

    l3d_mat4x4_mulConst(&m_tmp2, &m_tmp2, l3d_floatToRational( 1.0f - cosf( l3d_rationalToFloat(angle_rad) ) ));    // m_tmp2 = (1-cos(angle_rad)) * m_tmp2

    l3d_mat4x4_addMatrix(&m_tmp1, &m_tmp1, &m_tmp2);    // m_tmp1 += m_tmp2

    l3d_mat4x4_mulMatrix(&m_tmp1, &m_tmp1, &m_t);       // m_tmp1 *= m_t

    l3d_mat4x4_addMatrix(m, &m_i, &m_tmp1);             // m = m_i + m_tmp1

    // l3d_mat4x4_mulMatrix(&m_tmp1, );

// #ifdef L3D_USE_FIXED_POINT_ARITHMETIC

    // l3d_flp_t f_yaw = l3d_rationalToFloat(r.yaw);
    // l3d_flp_t f_pitch = l3d_rationalToFloat(r.pitch);
    // l3d_flp_t f_roll = l3d_rationalToFloat(r.roll);
    // m->m[0][0] = l3d_floatToFixed( cosf(f_yaw) * cosf(f_pitch) );
    // m->m[0][1] = l3d_floatToFixed( cosf(f_yaw) * sinf(f_pitch) * sinf(f_roll) - sinf(f_yaw) * cosf(f_roll) );
    // m->m[0][2] = l3d_floatToFixed(  );
    // m->m[0][3] = l3d_floatToFixed(  );
// #else

// #endif
    return L3D_OK;
}

// 
// Make a matrix representing a rotation about axis given by unit vector u
// Taken from:
// https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
// 
// m - output matrix
// u - unit vector (axis of rotation)
// angle_rad - angle in radians
// 
// l3d_err_t l3d_mat4x4_makeRot( l3d_mat4x4_t *m, const l3d_vec4_t *u, l3d_rtnl_t angle_rad, const l3d_vec4_t *pos) {
l3d_err_t l3d_mat4x4_makeRot( l3d_mat4x4_t *m, const l3d_vec4_t *u, l3d_rtnl_t angle_rad) {
    // TODO: remove this check from here; put it in caller instead
    // If u is not a unit vector (length > 1.0), abort
    if ((l3d_vec4_length(u) - l3d_floatToRational(1.0f)) > L3D_EPSILON_RTNL)
        return L3D_WRONG_PARAM;

    l3d_rtnl_t sin_theta = l3d_floatToRational( sinf( l3d_rationalToFloat(angle_rad) ) );
    l3d_rtnl_t cos_theta = l3d_floatToRational( cosf( l3d_rationalToFloat(angle_rad) ) );

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    l3d_rtnl_t ux2 = l3d_fixedMul(u->x, u->x);
    l3d_rtnl_t uy2 = l3d_fixedMul(u->y, u->y);
    l3d_rtnl_t uz2 = l3d_fixedMul(u->z, u->z);
    l3d_rtnl_t ux_uy = l3d_fixedMul(u->x, u->y);
    l3d_rtnl_t ux_uz = l3d_fixedMul(u->x, u->z);
    l3d_rtnl_t uy_uz = l3d_fixedMul(u->y, u->z);
    
    l3d_rtnl_t cos_diff = (l3d_floatToRational(1.0f)-cos_theta);

    // Row 0
    m->m[0][0] = l3d_fixedMul(ux2, cos_diff) + cos_theta;
    m->m[0][1] = l3d_fixedMul(ux_uy, cos_diff) - l3d_fixedMul(u->z, sin_theta);
    m->m[0][2] = l3d_fixedMul(ux_uz, cos_diff) + l3d_fixedMul(u->y, sin_theta);
    m->m[0][3] = l3d_floatToRational(0.0f);
    // Row 1
    m->m[1][0] = l3d_fixedMul(ux_uy, cos_diff) + l3d_fixedMul(u->z, sin_theta);
    m->m[1][1] = l3d_fixedMul(uy2, cos_diff) + cos_theta;
    m->m[1][2] = l3d_fixedMul(uy_uz, cos_diff) - l3d_fixedMul(u->x, sin_theta);
    m->m[1][3] = l3d_floatToRational(0.0f);
    // Row 2
    m->m[2][0] = l3d_fixedMul(ux_uz, cos_diff) - l3d_fixedMul(u->y, sin_theta);
    m->m[2][1] = l3d_fixedMul(uy_uz, cos_diff) + l3d_fixedMul(u->x, sin_theta);
    m->m[2][2] = l3d_fixedMul(uz2, cos_diff) + cos_theta;
    m->m[2][3] = l3d_floatToRational(0.0f);
    // Row 3
    // m->m[3][0] = pos->x;
    // m->m[3][1] = pos->y;
    // m->m[3][2] = pos->z;
    m->m[3][0] = l3d_floatToRational(0.0f);
    m->m[3][1] = l3d_floatToRational(0.0f);
    m->m[3][2] = l3d_floatToRational(0.0f);
    m->m[3][3] = l3d_floatToRational(1.0f);
#else
    l3d_rtnl_t ux2 = u->x * u->x;
    l3d_rtnl_t uy2 = u->y * u->y;
    l3d_rtnl_t uz2 = u->z * u->z;

    l3d_rtnl_t ux_uy = (u->x * u->y);
    l3d_rtnl_t ux_uz = (u->x * u->z);
    l3d_rtnl_t uy_uz = (u->y * u->z);
    
    l3d_rtnl_t cos_diff = (1.0f)-cos_theta;

    // Row 0
    m->m[0][0] = (ux2 * cos_diff) + cos_theta;
    m->m[0][1] = (ux_uy * cos_diff) - (u->z * sin_theta);
    m->m[0][2] = (ux_uz * cos_diff) + (u->y * sin_theta);
    m->m[0][3] = l3d_floatToRational(0.0f);
    // Row 1
    m->m[1][0] = (ux_uy * cos_diff) + (u->z * sin_theta);
    m->m[1][1] = (uy2 * cos_diff) + cos_theta;
    m->m[1][2] = (uy_uz * cos_diff) - (u->x * sin_theta);
    m->m[1][3] = l3d_floatToRational(0.0f);
    // Row 2
    m->m[2][0] = (ux_uz * cos_diff) - (u->y * sin_theta);
    m->m[2][1] = (uy_uz * cos_diff) + (u->x * sin_theta);
    m->m[2][2] = (uz2 * cos_diff) + cos_theta;
    m->m[2][3] = l3d_floatToRational(0.0f);
    // Row 3
    // m->m[3][0] = pos->x;
    // m->m[3][1] = pos->y;
    // m->m[3][2] = pos->z;
    m->m[3][0] = l3d_floatToRational(0.0f);
    m->m[3][1] = l3d_floatToRational(0.0f);
    m->m[3][2] = l3d_floatToRational(0.0f);
    m->m[3][3] = l3d_floatToRational(1.0f);
#endif

    

    // L3D_DEBUG_PRINT("cos_theta = %.3f\n", l3d_rationalToFloat(cos_theta));
    // L3D_DEBUG_PRINT("uz2 = %.3f\n", l3d_rationalToFloat(uz2));
    // L3D_DEBUG_PRINT("l3d_fixedMul(uz2, (1-cos_theta)) = %.3f\n", l3d_rationalToFloat(l3d_fixedMul(uz2, (1-cos_theta))));
    // L3D_DEBUG_PRINT("m->m[2][2] = %.3f\n", l3d_rationalToFloat(m->m[2][2]));

    return L3D_OK;
}

// 
// W.I.P., not used yet
// 
// Returns true if gimbal lock detected, false otherwise
// m - general rotation matrix, e.g. obtained from l3d_mat4x4_makeRot()
// 
bool l3d_mat4x4_getEulerAngles( const l3d_mat4x4_t *m, l3d_rot_t *rot ) {
    // rot->pitch = l3d_floatToRational( asinf( -1 * l3d_rationalToFloat(m->m[2][0]) ) );

    // if (m->m[2][0] - l3d_floatToRational(1.0f) < L3D_EPSILON_RTNL) {
    //     // Gimbal lock case
    //     rot->yaw = l3d_floatToRational( atan2f( -1*l3d_rationalToFloat(m->m[0][1]), l3d_rationalToFloat(m->m[1][1]) ) );
    //     rot->roll = 0;
    //     return true;
    // }
    // rot->yaw = l3d_floatToRational( atan2f( l3d_rationalToFloat(m->m[0][1]), l3d_rationalToFloat(m->m[1][1]) ) );
    // rot->roll = l3d_floatToRational( atan2f( l3d_rationalToFloat(m->m[2][1]), l3d_rationalToFloat(m->m[2][2]) ) );
    // return false;

    // Extract yaw (Z-axis rotation)
    rot->yaw = l3d_floatToRational( atan2f( l3d_rationalToFloat(m->m[2][1]), l3d_rationalToFloat(m->m[2][2]) ) );

    // Extract pitch (X-axis rotation), clamp to avoid gimbal lock
    l3d_rtnl_t sin_pitch = -m->m[2][0];
    if (sin_pitch <= l3d_floatToRational(-1.0f)) {
        rot->pitch = l3d_floatToRational( -M_PI / 2.0f );
    } else if (sin_pitch >= l3d_floatToRational(1.0f)) {
        rot->pitch = l3d_floatToRational( M_PI / 2.0f );
    } else {
        rot->pitch = l3d_floatToRational( asinf( l3d_rationalToFloat(sin_pitch)) );
    }

    // Extract roll (Y-axis rotation)
    rot->roll = atan2f(m->m[1][0], m->m[0][0]);

    return false;
}

void l3d_mat4x4_makeTranslation( l3d_mat4x4_t *m, const l3d_vec4_t *delta_pos ){
    l3d_mat4x4_makeEmpty( m );

    m->m[0][0] = l3d_floatToRational( 1.0f );
    m->m[1][1] = l3d_floatToRational( 1.0f );
    m->m[2][2] = l3d_floatToRational( 1.0f );
    m->m[3][3] = l3d_floatToRational( 1.0f );
    m->m[3][0] = delta_pos->x;
    m->m[3][1] = delta_pos->y;
    m->m[3][2] = delta_pos->z;
}

void l3d_mat4x4_makeProjection( l3d_mat4x4_t *m, l3d_rtnl_t fov_degrees, l3d_rtnl_t aspect_ratio, l3d_rtnl_t near_plane, l3d_rtnl_t far_plane ){
    l3d_mat4x4_makeEmpty( m );
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    // Fov coefficient in radians
    l3d_rtnl_t fov_rad = l3d_fixedDiv( l3d_floatToFixed( 1.0f ), l3d_floatToFixed( tanf( l3d_fixedToFloat( fov_degrees ) * 0.5f / 180.0f * 3.14159f ) ) );
    
    m->m[0][0] = l3d_fixedMul( aspect_ratio, fov_rad );
    m->m[1][1] = fov_rad;
    m->m[2][2] = l3d_fixedDiv( far_plane, ( far_plane - near_plane ) );
    m->m[3][2] = l3d_fixedDiv( l3d_fixedMul( -far_plane, near_plane ), ( far_plane - near_plane ) );
    // According to an answer here:
    // https://stackoverflow.com/questions/67184218/how-do-i-use-a-left-handed-coordinate-system-for-rendering
    //  m->m[2][3] =  1 for left-handed  coordinate system
    //  m->m[2][3] = -1 for right-handed coordinate system
    m->m[2][3] = l3d_floatToFixed( -1.0f );
    m->m[3][3] = l3d_floatToFixed( 0.0f );
#else
    // Fov coefficient in radians
    l3d_rtnl_t fov_rad = 1.0f / tanf( fov_degrees * 0.5f / 180.0f * 3.14159f );
    
    m->m[0][0] = aspect_ratio * fov_rad;
    m->m[1][1] = fov_rad;
    m->m[2][2] = far_plane / ( far_plane - near_plane );
    m->m[3][2] = ( -far_plane * near_plane ) / ( far_plane - near_plane );
    m->m[2][3] = -1.0f;
    m->m[3][3] = 0.0f;
#endif
}

void l3d_mat4x4_mulMatrix( l3d_mat4x4_t *m_out, const l3d_mat4x4_t *m1, const l3d_mat4x4_t *m2 ){
    for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
            m_out->m[r][c] = l3d_fixedMul( m1->m[r][0], m2->m[0][c] ) + l3d_fixedMul( m1->m[r][1], m2->m[1][c] ) + l3d_fixedMul( m1->m[r][2], m2->m[2][c] ) + l3d_fixedMul( m1->m[r][3], m2->m[3][c] );
#else
			m_out->m[r][c] = m1->m[r][0] * m2->m[0][c] + m1->m[r][1] * m2->m[1][c] + m1->m[r][2] * m2->m[2][c] + m1->m[r][3] * m2->m[3][c];
#endif
}

#ifdef L3D_CAMERA_MOVABLE
//  pos - where the object should be
//  target - "forward" vector for that object
//  up - "up" vector
void l3d_mat4x4_pointAt( l3d_mat4x4_t *m_out, const l3d_vec4_t *pos, const l3d_vec4_t *target, const l3d_vec4_t *up ){
    // Calculate new forward direction:
    // Z-axis
    l3d_vec4_t newForward = l3d_vec4_sub( target, pos );
    // l3d_vec4_t newForward = l3d_vec4_sub( pos, target );
    newForward = l3d_vec4_normalise( &newForward );
    // X-axis
    l3d_vec4_t newRight = l3d_vec4_crossProduct( up, &newForward );
    // l3d_vec4_t newRight = l3d_vec4_crossProduct( &newForward, up );
    newRight = l3d_vec4_normalise( &newRight );
    // Y-axis
    l3d_vec4_t newUp = l3d_vec4_crossProduct( &newForward, &newRight );
    // l3d_vec4_t newUp = l3d_vec4_crossProduct( &newRight, &newForward );
    newUp = l3d_vec4_normalise( &newUp );

    /*
    // Calculate new up direction:
    // Y-axis
    l3d_vec4_t a = vectorMul( &newForward, vectorDotProduct( up, &newForward ) );
    // DEBUG_PRINT( "a:\n" );
	// vec3d_print( &a, 1 );
    l3d_vec4_t newUp = vectorSub( up, &a );
    // DEBUG_PRINT( "newUp:\n" );
	// vec3d_print( &newUp, 1 );
    newUp = vectorNormalise( &newUp );
    // DEBUG_PRINT( "newUp after norm:\n" );
	// vec3d_print( &newUp, 1 );

    // Calculate new right direction:
    // X-axis
    l3d_vec4_t newRight = vectorCrossProduct( &newUp, &newForward );
    // DEBUG_PRINT( "newRight:\n" );
	// vec3d_print( &newRight, 1 );
    */

   l3d_vec4_t neye;    // -eye (negative eye) vector
   neye.x = -pos->x;
   neye.y = -pos->y;
   neye.z = -pos->z;

    // Construct Dimensioning and Translation Matrix	
    m_out->m[0][0] = newRight.x;	    m_out->m[0][1] = newRight.y;	    m_out->m[0][2] = newRight.z;	    m_out->m[0][3] = l3d_floatToRational(0.0f);
	m_out->m[1][0] = newUp.x;		    m_out->m[1][1] = newUp.y;		    m_out->m[1][2] = newUp.z;		    m_out->m[1][3] = l3d_floatToRational(0.0f);
	m_out->m[2][0] = newForward.x;	    m_out->m[2][1] = newForward.y;	    m_out->m[2][2] = newForward.z;	    m_out->m[2][3] = l3d_floatToRational(0.0f);
	m_out->m[3][0] = pos->x;			m_out->m[3][1] = pos->y;			m_out->m[3][2] = pos->z;			m_out->m[3][3] = l3d_floatToRational(1.0f);

}

// WIP:
void l3d_mat4x4_lookAtRH( l3d_mat4x4_t *m_out, l3d_vec4_t *eye, l3d_vec4_t *target, l3d_vec4_t *up );
void l3d_mat4x4_lookAtLH( l3d_mat4x4_t *m_out, l3d_vec4_t *eye, l3d_vec4_t *target, l3d_vec4_t *up );
void l3d_mat4x4_FPS( l3d_mat4x4_t *m_out, l3d_vec4_t *pos, l3d_rtnl_t pitch, l3d_rtnl_t yaw );

// Works only for Rotation/Translation Matrices
void l3d_mat4x4_quickInverse( l3d_mat4x4_t *m_out, const l3d_mat4x4_t *m ){
    l3d_mat4x4_makeEmpty( m_out );
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
	m_out->m[0][0] = m->m[0][0]; m_out->m[0][1] = m->m[1][0]; m_out->m[0][2] = m->m[2][0]; m_out->m[0][3] = l3d_floatToFixed(0.0f);
	m_out->m[1][0] = m->m[0][1]; m_out->m[1][1] = m->m[1][1]; m_out->m[1][2] = m->m[2][1]; m_out->m[1][3] = l3d_floatToFixed(0.0f);
	m_out->m[2][0] = m->m[0][2]; m_out->m[2][1] = m->m[1][2]; m_out->m[2][2] = m->m[2][2]; m_out->m[2][3] = l3d_floatToFixed(0.0f);
    m_out->m[3][0] = -( l3d_fixedMul( m->m[3][0], m_out->m[0][0] ) + l3d_fixedMul( m->m[3][1], m_out->m[1][0] ) + l3d_fixedMul( m->m[3][2], m_out->m[2][0] ) );
    m_out->m[3][1] = -( l3d_fixedMul( m->m[3][0], m_out->m[0][1] ) + l3d_fixedMul( m->m[3][1], m_out->m[1][1] ) + l3d_fixedMul( m->m[3][2], m_out->m[2][1] ) );
    m_out->m[3][2] = -( l3d_fixedMul( m->m[3][0], m_out->m[0][2] ) + l3d_fixedMul( m->m[3][1], m_out->m[1][2] ) + l3d_fixedMul( m->m[3][2], m_out->m[2][2] ) );
    m_out->m[3][3] = l3d_floatToFixed( 1.0f );
#else
    m_out->m[0][0] = m->m[0][0]; m_out->m[0][1] = m->m[1][0]; m_out->m[0][2] = m->m[2][0]; m_out->m[0][3] = 0.0f;
	m_out->m[1][0] = m->m[0][1]; m_out->m[1][1] = m->m[1][1]; m_out->m[1][2] = m->m[2][1]; m_out->m[1][3] = 0.0f;
	m_out->m[2][0] = m->m[0][2]; m_out->m[2][1] = m->m[1][2]; m_out->m[2][2] = m->m[2][2]; m_out->m[2][3] = 0.0f;
	m_out->m[3][0] = -(m->m[3][0] * m_out->m[0][0] + m->m[3][1] * m_out->m[1][0] + m->m[3][2] * m_out->m[2][0]);
	m_out->m[3][1] = -(m->m[3][0] * m_out->m[0][1] + m->m[3][1] * m_out->m[1][1] + m->m[3][2] * m_out->m[2][1]);
	m_out->m[3][2] = -(m->m[3][0] * m_out->m[0][2] + m->m[3][1] * m_out->m[1][2] + m->m[3][2] * m_out->m[2][2]);
	m_out->m[3][3] = 1.0f;
#endif
}
#endif	// L3D_CAMERA_MOVABLE

/* 
// The following will not compile 
// Screen clipping may be thought about in the future,
// but it is not the priority

#ifdef L3D_USE_SCREEN_CLIPPING
// Utility

// 
// plane_p - point defining the plane
// plane_n - normal defining the plane
// line_start - first point defining the line
// line_end - second point defining the line
// 
// Returns a point where line intersects with plane given by point and normal
// 
l3d_vec4_t l3d_intersect_plane(const l3d_vec4_t *plane_p, l3d_vec4_t *plane_n, const l3d_vec4_t *line_start, const l3d_vec4_t *line_end) {
    *plane_n = l3d_vec4_normalise(plane_n);
    l3d_rtnl_t plane_d = -l3d_vec4_dotProduct(plane_n, plane_p);
    l3d_rtnl_t ad = l3d_vec4_dotProduct(line_start, plane_n);
    l3d_rtnl_t bd = l3d_vec4_dotProduct(line_end, plane_n);
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    l3d_rtnl_t t = l3d_fixedDiv( (-plane_d - ad), (bd- ad) );
#else
    l3d_rtnl_t t = (-plane_d - ad) / (bd- ad);
#endif  // L3D_USE_FIXED_POINT_ARITHMETIC
    l3d_vec4_t line_start_to_end = l3d_vec4_sub(line_end, line_start);
    l3d_vec4_t line_to_intersect = l3d_vec4_mul(&line_start_to_end, t);
    return l3d_vec4_add(line_start, &line_to_intersect);
}

// 
// Returns signed shortest distance from point p to plane defined by its normal plane_n and a point plane_p.
// Plane normal must be normalised
// 
l3d_rtnl_t l3d_point_to_plane_distance(const l3d_vec4_t *p, const l3d_vec4_t *plane_p, const l3d_vec4_t *plane_n ) {
    l3d_vec4_t n = l3d_vec4_normalise(p);
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return (l3d_fixedMul(plane_n->x, p->x) + l3d_fixedMul(plane_n->y, p->y) + l3d_fixedMul(plane_n->z, p->z) - l3d_vec4_dotProduct(plane_n, plane_p));
#else
    return (plane_n->x * p->x + plane_n->y * p->y + plane_n->z * p->z - l3d_vec4_dotProduct(plane_n, plane_p));
#endif  // L3D_USE_FIXED_POINT_ARITHMETIC
}

// 
// Clip triangle in world space
// Returns how many triangles are returned by this function
// 
uint8_t l3d_clip_tri_against_plane(l3d_scene_t *scene, l3d_vec4_t *plane_p, l3d_vec4_t *plane_n, l3d_tri_t *in_tri, l3d_tri_t *out_tri1, l3d_tri_t *out_tri2) {
    *plane_p = l3d_vec4_normalise(plane_n);

    // Classify whether the point is inside or outside of the triangle??
    // Calculate the distance between point and the plane
    // Look at the sign of the distance to figure out if the point is inside or outside of the plane

    // Create two temp arrays to classify points either side of plane
    // If distance sign is positive, point lies on the inside of the plane
    l3d_vec4_t *inside_points[3];
    l3d_vec4_t *outside_points[3];
    uint8_t inside_point_cnt = 0;
    uint8_t outside_point_cnt = 0;

    // Get signed distance of each point in triangle to plane
    // l3d_tri_t in_tri = scene->tris[in_tri_idx];
    // Get each point
    l3d_vec4_t p0 = scene->vertices_world[in_tri->verts_ids[0]];
    l3d_vec4_t p1 = scene->vertices_world[in_tri->verts_ids[1]];
    l3d_vec4_t p2 = scene->vertices_world[in_tri->verts_ids[2]];
    // Get the distances
    l3d_rtnl_t d0 = l3d_point_to_plane_distance(&p0, plane_p, plane_n);
    l3d_rtnl_t d1 = l3d_point_to_plane_distance(&p1, plane_p, plane_n);
    l3d_rtnl_t d2 = l3d_point_to_plane_distance(&p2, plane_p, plane_n);

    if (d0 >= l3d_floatToRational(0.0f))
        inside_points[inside_point_cnt++] = &p0;
    else
        outside_points[outside_point_cnt++] = &p0;
    if (d1 >= l3d_floatToRational(0.0f))
        inside_points[inside_point_cnt++] = &p1;
    else
        outside_points[outside_point_cnt++] = &p1;
    if (d2 >= l3d_floatToRational(0.0f))
        inside_points[inside_point_cnt++] = &p2;
    else
        outside_points[outside_point_cnt++] = &p2;
    
    // Classify the triangle points and break the input triangle into
    // smaller output triangles if required
    // There are 4 possible outcomes
    if (inside_point_cnt == 0) {
        // All points lie on the outside of plane, so clip whole triangle
        return 0;   // no returned triangles are valid
    }
    else if (inside_point_cnt == 3) {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simpy pass through
        out_tri1 = in_tri;
        return 1;   // just the one original triangle is valid
    }
    else if (inside_point_cnt == 1 && outside_point_cnt == 2) {
        // Triangle should be clipped
        // As two points lie outside the plane,
        // the triangle simply becomes a smaller triangle

        // Copy tri data
        // out_tri1->is_visible = in_tri->is_visible

        // The inside point is valid
        out_tri1.verts[0] = *inside_points[0];

        // Two new points are at the intersections with the plane
        out_tri1.verts[1] = l3d_intersect_plane(plane_p, plane_n, &inside_points[0], &outside_points[0]);
        out_tri1.verts[2] = l3d_intersect_plane(plane_p, plane_n, &inside_points[0], &outside_points[1]);

        return 1;   // return the newly formed triangle
    }

    else if (inside_point_cnt == 1 && outside_point_cnt == 2) {
        // The clipped triangle becomes a quad,
        // so create 2 triangles

        // Copy tri data
        // out_tri1->is_visible = in_tri->is_visible
        // out_tri2->is_visible = in_tri->is_visible

        // The first triangle consists of the two inside points and
        // a new point determined by the intersection point with the plane
        out_tri1.verts[0] = *inside_points[0];
        out_tri1.verts[1] = *inside_points[1];
        out_tri1.verts[2] = l3d_intersect_plane(plane_p, plane_n, &inside_points[0], &outside_points[0]);

        // The second triangle is composed of one of the inside points,
        // a new point at the intersection with the plane,
        // and the newly created point above
        out_tri2.verts[0] = *inside_points[1];
        out_tri2.verts[1] = out_tri1.verts[2];
        out_tri2.verts[2] = l3d_intersect_plane(plane_p, plane_n, &inside_points[1], &outside_points[1]);

        return 2;   // two triangles are valid
    }
}

#endif  // L3D_USE_SCREEN_CLIPPING

*/ // triangle clipping
