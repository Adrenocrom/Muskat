#include	"muskat.h"

real4x4	real4x4Translation(const real3& v) {
	return real4x4(1.0f, 0.0f, 0.0f, 0.0f,
			         0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						v.x,  v.y,  v.z,  1.0f);
}
real4x4	real4x4RotationX(const real f) {
	real4x4 mResult;

	mResult.m11 = 1.0f; mResult.m12 = 0.0f; mResult.m13 = 0.0f; mResult.m14 = 0.0f;
	mResult.m21 = 0.0f;                                         mResult.m24 = 0.0f;
	mResult.m31 = 0.0f;                                         mResult.m34 = 0.0f;
	mResult.m41 = 0.0f; mResult.m42 = 0.0f; mResult.m43 = 0.0f; mResult.m44 = 1.0f;
	mResult.m22 = mResult.m33 = cosf(f);
	mResult.m23 = sinf(f);
	mResult.m32 = -mResult.m23;
	return mResult;
}
real4x4	real4x4RotationY(const real f) {
	real4x4 mResult;

	mResult.m12 = 0.0f;                     mResult.m14 = 0.0f;
	mResult.m21 = 0.0f; mResult.m22 = 1.0f; mResult.m23 = 0.0f; mResult.m24 = 0.0f;
	mResult.m32 = 0.0f;                     mResult.m34 = 0.0f;
	mResult.m41 = 0.0f; mResult.m42 = 0.0f; mResult.m43 = 0.0f; mResult.m44 = 1.0f;
	mResult.m11 = mResult.m33 = cosf(f);
	mResult.m31 = sinf(f);
	mResult.m13 = -mResult.m31;
	return mResult;
}
real4x4	real4x4RotationZ(const real f) {
	real4x4 mResult;

	mResult.m13 = 0.0f; mResult.m14 = 0.0f;
	mResult.m23 = 0.0f; mResult.m24 = 0.0f;
	mResult.m31 = 0.0f; mResult.m32 = 0.0f; mResult.m33 = 1.0f; mResult.m34 = 0.0f;
	mResult.m41 = 0.0f; mResult.m42 = 0.0f; mResult.m43 = 0.0f; mResult.m44 = 1.0f;
	mResult.m11 = mResult.m22 = cosf(f);
	mResult.m12 = sinf(f);
	mResult.m21 = -mResult.m12;
	return mResult;
}
real4x4	real4x4Rotation(const real x, const real y, const real z) {
	return real4x4RotationZ(z) *
			 real4x4RotationX(x) *
			 real4x4RotationY(y);
}
real4x4	real4x4Rotation(const real3& v) {
	return real4x4RotationZ(v.z) *
			 real4x4RotationX(v.x) *
			 real4x4RotationY(v.y);
}
real4x4	real4x4RotationAxis(const real3& v, const real f) {
	const real fSin = sinf(-f);
	const real fCos = cosf(-f);
	const real fOneMinusCos = 1.0f - fCos;

	// Achsenvektor normalisieren
	const real3 vAxis(normalize(v));
	
	// Matrix erstellen
	return real4x4((vAxis.x * vAxis.x) * fOneMinusCos + fCos,
		            (vAxis.x * vAxis.y) * fOneMinusCos - (vAxis.z * fSin),
						(vAxis.x * vAxis.z) * fOneMinusCos + (vAxis.y * fSin),
						0.0f,
						(vAxis.y * vAxis.x) * fOneMinusCos + (vAxis.z * fSin),
						(vAxis.y * vAxis.y) * fOneMinusCos + fCos,
						(vAxis.y * vAxis.z) * fOneMinusCos - (vAxis.x * fSin),
						0.0f,
						(vAxis.z * vAxis.x) * fOneMinusCos - (vAxis.y * fSin),
						(vAxis.z * vAxis.y) * fOneMinusCos + (vAxis.x * fSin),
						(vAxis.z * vAxis.z) * fOneMinusCos + fCos,
						0.0f,
						0.0f,
						0.0f,
						0.0f,
						1.0f);
}
real4x4	real4x4Scaling(const real3& v) {
	return real4x4(v.x,  0.0f, 0.0f, 0.0f,
			         0.0f, v.y,  0.0f, 0.0f,
						0.0f, 0.0f, v.z,  0.0f,
						0.0f, 0.0f, 0.0f, 1.0f);
}
real4x4	real4x4Axes(const real3& vXAxis, const real3& vYAxis, const real3& vZAxis) {
	return real4x4(vXAxis.x, vXAxis.y, vXAxis.z, 0.0f,
			         vYAxis.x, vYAxis.y, vYAxis.z, 0.0f,
						vZAxis.x, vZAxis.y, vZAxis.z, 0.0f,
						0.0f,     0.0f,     0.0f,     1.0f);
}
real		real4x4Det(const real4x4& m) {
	return m.m11 * (m.m22 * m.m33 - m.m23 * m.m32) -
	       m.m12 * (m.m21 * m.m33 - m.m23 * m.m31) +
			 m.m13 * (m.m21 * m.m32 - m.m22 * m.m31);
}
real4x4	real4x4Invert(const real4x4& m) {
	real fInvDet = real4x4Det(m);
	if(fInvDet == 0.0f) return real4x4Identity();
	fInvDet = 1.0f / fInvDet;

	// Invertierte Matrix berechnen
	real4x4 mResult;
	mResult.m11 =  fInvDet * (m.m22 * m.m33 - m.m23 * m.m32);
	mResult.m12 = -fInvDet * (m.m12 * m.m33 - m.m13 * m.m32);
	mResult.m13 =  fInvDet * (m.m12 * m.m23 - m.m13 * m.m22);
	mResult.m14 =  0.0f;
	mResult.m21 = -fInvDet * (m.m21 * m.m33 - m.m23 * m.m31);
	mResult.m22 =  fInvDet * (m.m11 * m.m33 - m.m13 * m.m31);
	mResult.m23 = -fInvDet * (m.m11 * m.m23 - m.m13 * m.m21);
	mResult.m24 =  0.0f;
	mResult.m31 =  fInvDet * (m.m21 * m.m32 - m.m22 * m.m31);
	mResult.m32 = -fInvDet * (m.m11 * m.m32 - m.m12 * m.m31);
	mResult.m33 =  fInvDet * (m.m11 * m.m22 - m.m12 * m.m21);
	mResult.m34 =  0.0f;
	mResult.m41 = -(m.m41 * mResult.m11 + m.m42 * mResult.m21 + m.m43 * mResult.m31);
	mResult.m42 = -(m.m41 * mResult.m12 + m.m42 * mResult.m22 + m.m43 * mResult.m32);
	mResult.m43 = -(m.m41 * mResult.m13 + m.m42 * mResult.m23 + m.m43 * mResult.m33);
	mResult.m44 =  1.0f;
	return mResult;
}
real4x4	real4x4Transpose(const real4x4& m) {
	return real4x4(m.m11, m.m21, m.m31, m.m41,
			         m.m12, m.m22, m.m32, m.m42,
						m.m13, m.m23, m.m33, m.m43,
						m.m14, m.m24, m.m34, m.m44);
}
real4x4	real4x4Projection(const real fFOV, const real fAspect, const real fNearPlane, const real fFarPlane) {
	const real s = 1.0f / tanf(fFOV * 0.5f);
	const real Q = fFarPlane / (fFarPlane - fNearPlane);

	return real4x4(s / fAspect, 0.0f, 0.0f, 0.0f,
					   0.0f, s, 0.0f, 0.0f,
						0.0f, 0.0f, Q, 1.0f,
						0.0f, 0.0f, -Q * fNearPlane, 0.0f);
}

real4x4	real4x4Projection2(const real fFOV, const real fAspect, const real fNearPlane, const real fFarPlane) {
    real4x4 out;

    const real
        y_scale = SI_COT(SI_DEG_TO_RAD(fFOV / 2)),
        x_scale = y_scale / fAspect,
        frustum_length = fFarPlane - fNearPlane;

    out.n[0] = x_scale;
    out.n[5] = y_scale;
    out.n[10] = -((fFarPlane + fNearPlane) / frustum_length);
    out.n[11] = -1;
    out.n[14] = -((2 * fNearPlane * fFarPlane) / frustum_length);

    return out;
}

real4x4	real4x4Camera(const real3& vPos, const real3& vLookAt, const real3& vUp) {
	real3 vZAxis(normalize(vLookAt - vPos));

	// Die x-Achse ist das Kreuzprodukt aus y- und z-Achse
	real3 vXAxis(normalize(cross(vUp, vZAxis)));

	// y-Achse berechnen
	real3 vYAxis(normalize(cross(vZAxis, vXAxis)));
	
	// Rotationsmatrix erzeugen und die Translationsmatrix mit ihr multiplizieren
	return real4x4Translation(-vPos) *
			 real4x4(vXAxis.x, vYAxis.x, vZAxis.x, 0.0f,
						vXAxis.y, vYAxis.y, vZAxis.y, 0.0f,
						vXAxis.z, vYAxis.z, vZAxis.z, 0.0f,
						0.0f,     0.0f,     0.0f,     1.0f);
}
real4x4	real4x4ToTex2DMatrix(const real4x4& m) {
	return real4x4(m.m11, m.m12, m.m14, 0.0f,
			         m.m21, m.m22, m.m24, 0.0f,
						m.m41, m.m42, m.m44, 0.0f,
						0.0f,  0.0f,  0.0f,  1.0f);
}
