#pragma once

#include "WfConfig.h"

typedef struct _D3DMATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
		};
		float m[4][4];
	};
} D3DMATRIX;

typedef struct D3DXFLOAT16 {
	WORD Value;
} D3DXFLOAT16, *LPD3DXFLOAT16;


typedef struct D3DXMATRIX : public D3DMATRIX
{
public:
	D3DXMATRIX() {};
	D3DXMATRIX(CONST FLOAT *);
	D3DXMATRIX(CONST D3DMATRIX&);
	D3DXMATRIX(CONST D3DXFLOAT16 *);
	D3DXMATRIX(FLOAT _11, FLOAT _12, FLOAT _13, FLOAT _14,
		FLOAT _21, FLOAT _22, FLOAT _23, FLOAT _24,
		FLOAT _31, FLOAT _32, FLOAT _33, FLOAT _34,
		FLOAT _41, FLOAT _42, FLOAT _43, FLOAT _44);


	// access grants
	FLOAT& operator () (UINT Row, UINT Col);
	FLOAT  operator () (UINT Row, UINT Col) const;

	// casting operators
	operator FLOAT* ();
	operator CONST FLOAT* () const;

	// assignment operators
	D3DXMATRIX& operator *= (CONST D3DXMATRIX&);
	D3DXMATRIX& operator += (CONST D3DXMATRIX&);
	D3DXMATRIX& operator -= (CONST D3DXMATRIX&);
	D3DXMATRIX& operator *= (FLOAT);
	D3DXMATRIX& operator /= (FLOAT);

	// unary operators
	D3DXMATRIX operator + () const;
	D3DXMATRIX operator - () const;

	// binary operators
	D3DXMATRIX operator * (CONST D3DXMATRIX&) const;
	D3DXMATRIX operator + (CONST D3DXMATRIX&) const;
	D3DXMATRIX operator - (CONST D3DXMATRIX&) const;
	D3DXMATRIX operator * (FLOAT) const;
	D3DXMATRIX operator / (FLOAT) const;

	friend D3DXMATRIX operator * (FLOAT, CONST D3DXMATRIX&);

	BOOL operator == (CONST D3DXMATRIX&) const;
	BOOL operator != (CONST D3DXMATRIX&) const;

} D3DXMATRIX, *LPD3DXMATRIX;

