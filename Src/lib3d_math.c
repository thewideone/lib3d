#include "../Inc/lib3d_math.h"
#include "../Inc/lib3d_core.h" // for l3d_errorHandler()
#include "../Inc/lib3d_util.h"  // for debug print
#include "math.h"
#include <string.h> // for memset()
#include <stdio.h> // for sprintf()

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

l3d_rtnl_t l3d_getZeroRtnl(void){
    return l3d_floatToRational(0.0f);
}

l3d_rot_t l3d_getZeroRot(void){
    return l3d_getRotFromFloat(0.0f, 0.0f, 0.0f);
}

l3d_vec4_t l3d_getZeroVec4(void){
    return l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);
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
#else
    l3d_rtnl_t ux2 = u->x * u->x;
    l3d_rtnl_t uy2 = u->y * u->y;
    l3d_rtnl_t uz2 = u->z * u->z;
#endif

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

    // L3D_DEBUG_PRINT("cos_theta = %.3f\n", l3d_rationalToFloat(cos_theta));
    // L3D_DEBUG_PRINT("uz2 = %.3f\n", l3d_rationalToFloat(uz2));
    // L3D_DEBUG_PRINT("l3d_fixedMul(uz2, (1-cos_theta)) = %.3f\n", l3d_rationalToFloat(l3d_fixedMul(uz2, (1-cos_theta))));
    // L3D_DEBUG_PRINT("m->m[2][2] = %.3f\n", l3d_rationalToFloat(m->m[2][2]));

    return L3D_OK;
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
