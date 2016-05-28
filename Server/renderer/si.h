#ifndef SI_H
#define SI_H

#pragma once

typedef float real;
typedef unsigned int uint;

#define SI_MIN(a, b)			((a) < (b) ? (a) : (b))													// Minimum
#define SI_MAX(a, b)       ((a) > (b) ? (a) : (b))													// Maximum
#define SI_PI					(3.1415926535897932384626433832795f)	
#define SI_DEG_TO_RAD(x)	((x) * 0.0174532925199432957692369076848f)
#define SI_RAD_TO_DEG(x)	((x) * 57.295779513082320876798154814105f)					
#define SI_COT(x)				((real)(1.0/tan(x)))


struct real3 {
	real x;
	real y;
	real z;

	real3() : x(0.0), y(0.0), z(0.0) 														{}
	real3(const real _x, const real _y, const real _z) : x(_x), y(_y), z(_z)	{}

	real3& operator = (const real3& v) {x = v.x; y = v.y; z = v.z; return *this;}
	real3& operator += (const real3& v) {x += v.x; y += v.y; z += v.z; return *this;}
	real3& operator -= (const real3& v) {x -= v.x; y -= v.y; z -= v.z; return *this;}
	real3& operator *= (const real3& v) {x *= v.x; y *= v.y; z *= v.z; return *this;}
	real3& operator *= (const real f) {x *= f; y *= f; z *= f; return *this;}
	real3& operator /= (const real3& v) {x /= v.x; y /= v.y; z /= v.z; return *this;}
	real3& operator /= (const real f) {x /= f; y /= f; z /= f; return *this;}

	real3& clamp() {
		x = SI_MAX(SI_MIN(255.0, x), 0.0); 
		y = SI_MAX(SI_MIN(255.0, y), 0.0); 
		z = SI_MAX(SI_MIN(255.0, z), 0.0); 
		return *this;
	}
};

inline real3 operator + (const real3& a, const real3& b) {return real3(a.x + b.x, a.y + b.y, a.z + b.z);}
inline real3 operator + (const real3& v, const real f) {return real3(v.x + f, v.y + f, v.z + f);}
inline real3 operator - (const real3& a, const real3& b) {return real3(a.x - b.x, a.y - b.y, a.z - b.z);}
inline real3 operator - (const real3& v, const real f) {return real3(v.x - f, v.y - f, v.z - f);}
inline real3 operator - (const real3& v) {return real3(-v.x, -v.y, -v.z);}
inline real3 operator * (const real3& a, const real3& b) {return real3(a.x * b.x, a.y * b.y, a.z * b.z);}
inline real3 operator * (const real3& v, const real f) {return real3(v.x * f, v.y * f, v.z * f);}
inline real3 operator * (const real f, const real3& v) {return real3(v.x * f, v.y * f, v.z * f);}
inline real3 operator / (const real3& a, const real3& b) {return real3(a.x / b.x, a.y / b.y, a.z / b.z);}
inline real3 operator / (const real3& v, const real f) {return real3(v.x / f, v.y / f, v.z / f);}
inline real3 operator / (const real f, const real3& v) {return real3(f / v.x, f / v.y, f / v.z);}

// Vergleichsoperatoren
inline bool operator == (const real3& a, const real3& b) {if(a.x != b.x) return false; if(a.y != b.y) return false; return a.z == b.z;}
inline bool operator != (const real3& a, const real3& b) {if(a.x != b.x) return true; if(a.y != b.y) return true; return a.z != b.z;}

inline real  dot(const real3& a, const real3& b) {return a.x*b.x + a.y*b.y + a.z*b.z;}
inline real3 cross(const real3& v1, const real3& v2) {return real3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);}
inline real3 normalize(const real3& v) {return v / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);}
inline real	 clamp(const real& f) {return SI_MAX(SI_MIN(255.0, f), 0.0);}

inline real3 real3Min(const real3& v1, const real3& v2) {return real3(SI_MIN(v1.x, v2.x), SI_MIN(v1.y, v2.y), SI_MIN(v1.z, v2.z));}
inline real3 real3Max(const real3& v1, const real3& v2) {return real3(SI_MAX(v1.x, v2.x), SI_MAX(v1.y, v2.y), SI_MAX(v1.z, v2.z));}


inline uint real3MaxDim(const real3& v) {if(v.x >= v.y && v.x >= v.z)return 0;else if(v.y >= v.x && v.y >= v.z)return 1;else return 2;}

struct real4x4;
real4x4 real4x4Invert(const real4x4& m);

struct real4x4 {
	union {
		struct {
			real  m11, m12, m13, m14,	// Elemente der Matrix
					m21, m22, m23, m24,
					m31, m32, m33, m34,
					m41, m42, m43, m44;
		};
																							
		real		m[4][4];			// Zweidimensionales Array der Elemente
		real		n[16];				// Eindimensionales Array der Elemente
	};

	real4x4() {}

	real4x4(const real4x4& m) : m11(m.m11), m12(m.m12), m13(m.m13), m14(m.m14),
		                         m21(m.m21), m22(m.m22), m23(m.m23), m24(m.m24),
										 m31(m.m31), m32(m.m32), m33(m.m33), m34(m.m34),
										 m41(m.m41), m42(m.m42), m43(m.m43), m44(m.m44) {}
	real4x4(real _m11, real _m12, real _m13, real _m14,
			  real _m21, real _m22, real _m23, real _m24,
			  real _m31, real _m32, real _m33, real _m34,
 			  real _m41, real _m42, real _m43, real _m44) : m11(_m11), m12(_m12), m13(_m13), m14(_m14),
																	      m21(_m21), m22(_m22), m23(_m23), m24(_m24),
																		   m31(_m31), m32(_m32), m33(_m33), m34(_m34),
																		   m41(_m41), m42(_m42), m43(_m43), m44(_m44) {}
	operator real* () {return (real*)(n);}

	real& operator () (uint iRow, uint iColumn) {return m[iRow - 1][iColumn - 1];}
	real  operator () (uint iRow, uint iColumn) const {return m[iRow - 1][iColumn - 1];}

	real4x4& operator += (const real4x4& m) {
		m11 += m.m11; m12 += m.m12; m13 += m.m13; m14 += m.m14;
		m21 += m.m21; m22 += m.m22; m23 += m.m23; m24 += m.m24;
		m31 += m.m31; m32 += m.m32; m33 += m.m33; m34 += m.m34;
		m41 += m.m41; m42 += m.m42; m43 += m.m43; m44 += m.m44;
		return *this;
	}

	real4x4& operator -= (const real4x4& m) {
		m11 -= m.m11; m12 -= m.m12; m13 -= m.m13; m14 -= m.m14;
		m21 -= m.m21; m22 -= m.m22; m23 -= m.m23; m24 -= m.m24;
		m31 -= m.m31; m32 -= m.m32; m33 -= m.m33; m34 -= m.m34;
		m41 -= m.m41; m42 -= m.m42; m43 -= m.m43; m44 -= m.m44;
		return *this;
	}

	real4x4& operator *= (const real4x4& m) {
		return *this = real4x4(m.m11 * m11 + m.m21 * m12 + m.m31 * m13 + m.m41 * m14,
									  m.m12 * m11 + m.m22 * m12 + m.m32 * m13 + m.m42 * m14,
									  m.m13 * m11 + m.m23 * m12 + m.m33 * m13 + m.m43 * m14,
									  m.m14 * m11 + m.m24 * m12 + m.m34 * m13 + m.m44 * m14,
									  m.m11 * m21 + m.m21 * m22 + m.m31 * m23 + m.m41 * m24,
									  m.m12 * m21 + m.m22 * m22 + m.m32 * m23 + m.m42 * m24,
									  m.m13 * m21 + m.m23 * m22 + m.m33 * m23 + m.m43 * m24,
									  m.m14 * m21 + m.m24 * m22 + m.m34 * m23 + m.m44 * m24,
									  m.m11 * m31 + m.m21 * m32 + m.m31 * m33 + m.m41 * m34,
									  m.m12 * m31 + m.m22 * m32 + m.m32 * m33 + m.m42 * m34,
									  m.m13 * m31 + m.m23 * m32 + m.m33 * m33 + m.m43 * m34,
									  m.m14 * m31 + m.m24 * m32 + m.m34 * m33 + m.m44 * m34,
									  m.m11 * m41 + m.m21 * m42 + m.m31 * m43 + m.m41 * m44,
									  m.m12 * m41 + m.m22 * m42 + m.m32 * m43 + m.m42 * m44,
									  m.m13 * m41 + m.m23 * m42 + m.m33 * m43 + m.m43 * m44,
									  m.m14 * m41 + m.m24 * m42 + m.m34 * m43 + m.m44 * m44);
	}

	real4x4& operator *= (const real f) {
		m11 *= f; m12 *= f; m13 *= f; m14 *= f;
		m21 *= f; m22 *= f; m23 *= f; m24 *= f;
		m31 *= f; m32 *= f; m33 *= f; m34 *= f;
		m41 *= f; m42 *= f; m43 *= f; m44 *= f;
		return *this;
	}
														
	real4x4& operator /= (const real4x4& m) {return *this *= real4x4Invert(m);}
};

inline real4x4 operator + (const real4x4& a, const real4x4& b)	{return real4x4(a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13, a.m14 + b.m14, a.m21 + b.m21, a.m22 + b.m22, a.m23 + b.m23, a.m24 + b.m24, a.m31 + b.m31, a.m32 + b.m32, a.m33 + b.m33, a.m34 + b.m34, a.m41 + b.m41, a.m42 + b.m42, a.m43 + b.m43, a.m44 + b.m44);}
inline real4x4 operator - (const real4x4& a, const real4x4& b)	{return real4x4(a.m11 - b.m11, a.m12 - b.m12, a.m13 - b.m13, a.m14 - b.m14, a.m21 - b.m21, a.m22 - b.m22, a.m23 - b.m23, a.m24 - b.m24, a.m31 - b.m31, a.m32 - b.m32, a.m33 - b.m33, a.m34 - b.m34, a.m41 - b.m41, a.m42 - b.m42, a.m43 - b.m43, a.m44 - b.m44);}
inline real4x4 operator - (const real4x4& m)							{return real4x4(-m.m11, -m.m12, -m.m13, -m.m14, -m.m21, -m.m22, -m.m23, -m.m24, -m.m31, -m.m32, -m.m33, -m.m34, -m.m41, -m.m42, -m.m43, -m.m44);}

inline real4x4 operator * (const real4x4& a, const real4x4& b) {
	return real4x4(b.m11 * a.m11 + b.m21 * a.m12 + b.m31 * a.m13 + b.m41 * a.m14,
						b.m12 * a.m11 + b.m22 * a.m12 + b.m32 * a.m13 + b.m42 * a.m14,
						b.m13 * a.m11 + b.m23 * a.m12 + b.m33 * a.m13 + b.m43 * a.m14,
						b.m14 * a.m11 + b.m24 * a.m12 + b.m34 * a.m13 + b.m44 * a.m14,
						b.m11 * a.m21 + b.m21 * a.m22 + b.m31 * a.m23 + b.m41 * a.m24,
						b.m12 * a.m21 + b.m22 * a.m22 + b.m32 * a.m23 + b.m42 * a.m24,
						b.m13 * a.m21 + b.m23 * a.m22 + b.m33 * a.m23 + b.m43 * a.m24,
						b.m14 * a.m21 + b.m24 * a.m22 + b.m34 * a.m23 + b.m44 * a.m24,
						b.m11 * a.m31 + b.m21 * a.m32 + b.m31 * a.m33 + b.m41 * a.m34,
						b.m12 * a.m31 + b.m22 * a.m32 + b.m32 * a.m33 + b.m42 * a.m34,
						b.m13 * a.m31 + b.m23 * a.m32 + b.m33 * a.m33 + b.m43 * a.m34,
						b.m14 * a.m31 + b.m24 * a.m32 + b.m34 * a.m33 + b.m44 * a.m34,
						b.m11 * a.m41 + b.m21 * a.m42 + b.m31 * a.m43 + b.m41 * a.m44,
						b.m12 * a.m41 + b.m22 * a.m42 + b.m32 * a.m43 + b.m42 * a.m44,
						b.m13 * a.m41 + b.m23 * a.m42 + b.m33 * a.m43 + b.m43 * a.m44,
						b.m14 * a.m41 + b.m24 * a.m42 + b.m34 * a.m43 + b.m44 * a.m44);
}

inline real4x4 operator * (const real4x4& m, const real f) {
	return real4x4(m.m11 * f, m.m12 * f, m.m13 * f, m.m14 * f,
						m.m21 * f, m.m22 * f, m.m23 * f, m.m24 * f,
						m.m31 * f, m.m32 * f, m.m33 * f, m.m34 * f,
						m.m41 * f, m.m42 * f, m.m43 * f, m.m44 * f);
}

inline real4x4 operator * (const real f, const real4x4& m) {
	return real4x4(m.m11 * f, m.m12 * f, m.m13 * f, m.m14 * f,
						m.m21 * f, m.m22 * f, m.m23 * f, m.m24 * f,
						m.m31 * f, m.m32 * f, m.m33 * f, m.m34 * f,
						m.m41 * f, m.m42 * f, m.m43 * f, m.m44 * f);
}

inline real4x4 operator / (const real4x4& a, const real4x4& b) {return a * real4x4Invert(b);}

inline real4x4 operator / (const real4x4& m, const real f) {
	return real4x4(m.m11 / f, m.m12 / f, m.m13 / f, m.m14 / f,
						m.m21 / f, m.m22 / f, m.m23 / f, m.m24 / f,
						m.m31 / f, m.m32 / f, m.m33 / f, m.m34 / f,
						m.m41 / f, m.m42 / f, m.m43 / f, m.m44 / f);
}

inline bool operator == (const real4x4& a, const real4x4& b) {
	if(a.m11 != b.m11) return false;
	if(a.m12 != b.m12) return false;
	if(a.m13 != b.m13) return false;
	if(a.m14 != b.m14) return false;
	if(a.m21 != b.m21) return false;
	if(a.m22 != b.m22) return false;
	if(a.m23 != b.m23) return false;
	if(a.m24 != b.m24) return false;
	if(a.m31 != b.m31) return false;
	if(a.m32 != b.m32) return false;
	if(a.m33 != b.m33) return false;
	if(a.m34 != b.m34) return false;
	if(a.m41 != b.m41) return false;
	if(a.m42 != b.m42) return false;
	if(a.m43 != b.m43) return false;
	return a.m44 == b.m44;
}

inline bool operator != (const real4x4& a, const real4x4& b) {
	if(a.m11 != b.m11) return true;
	if(a.m12 != b.m12) return true;
	if(a.m13 != b.m13) return true;
	if(a.m14 != b.m14) return true;
	if(a.m21 != b.m21) return true;
	if(a.m22 != b.m22) return true;
	if(a.m23 != b.m23) return true;
	if(a.m24 != b.m24) return true;
	if(a.m31 != b.m31) return true;
	if(a.m32 != b.m32) return true;
	if(a.m33 != b.m33) return true;
	if(a.m34 != b.m34) return true;
	if(a.m41 != b.m41) return true;
	if(a.m42 != b.m42) return true;
	if(a.m43 != b.m43) return true;
	return a.m44 != b.m44;
}

inline real4x4	real4x4Identity() {return real4x4(1.0f, 0.0f, 0.0f, 0.0f,
																 0.0f, 1.0f, 0.0f, 0.0f,
																 0.0f, 0.0f, 1.0f, 0.0f,
																 0.0f, 0.0f, 0.0f, 1.0f);}	
real4x4	real4x4Translation(const real3& v);
real4x4	real4x4RotationX(const real f);
real4x4	real4x4RotationY(const real f);
real4x4	real4x4RotationZ(const real f);
real4x4	real4x4Rotation(const real x, const real y, const real z);
real4x4	real4x4Rotation(const real3& v);
real4x4	real4x4RotationAxis(const real3& v, const real f);
real4x4	real4x4Scaling(const real3& v);
real4x4	real4x4Axes(const real3& vXAxis, const real3& vYAxis, const real3& vZAxis);
real	real4x4Det(const real4x4& m);
real4x4	real4x4Invert(const real4x4& m);
real4x4	real4x4Transpose(const real4x4& m);
real4x4	real4x4Projection(const real fFOV, const real fAspect, const real fNearPlane, const real fFarPlane);
real4x4	real4x4Projection2(const real fFOV, const real fAspect, const real fNearPlane, const real fFarPlane);
real4x4	real4x4Camera(const real3& vPos, const real3& vLookAt, const real3& vUp = real3(0.0f, 1.0f, 0.0f));
real4x4	real4x4ToTex2DMatrix(const real4x4& m);

struct rgb {
	uint r;
	uint g;
	uint b;
			
	rgb() : r(0), g(0), b(0) {}
	rgb(const uint _r, const uint _g, const uint _b) : r(_r), g(_g), b(_b) {}
					
	rgb& operator = (const real3& v) {
		r = (uint)clamp(v.x);
		g = (uint)clamp(v.y);
		b = (uint)clamp(v.z);
		return *this;
	}
};

#endif
