// Written by Jeffrey Chastine, 2012
#ifndef MATH_HELPER
#define MATH_HELPER

#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
//#include "MathHelper.h"
#define T0 0
#define T1 4
#define T2 8
#define T3 12
#define T4 1
#define T5 5
#define T6 9
#define T7 13
#define T8 2
#define T9 6
#define T10 10
#define T11 14
#define T12 3
#define T13 7
#define T14 11
#define T15 15

	// This is a column-order matrix multiply of matrices m1 and m2.
	static void matrixMult4x4 (GLfloat* result, GLfloat* m1, GLfloat* m2);
	static void matrixMult4x4Column (GLfloat* result, GLfloat* m1, GLfloat* m2);
	// Makes an identity matrix
	static void makeIdentity (GLfloat* result);

	// Returns a matrix that translates by x, y and z amount
	static void makeTranslate (GLfloat* result, GLfloat x, GLfloat y, GLfloat z);
	// Updates result to include a translate
	static void translateMatrixBy (GLfloat* result, GLfloat x, GLfloat y, GLfloat z);

	// Returns matrices that rotate about the X, Y and Z axes by a rotation amount (radians)
	static void makeRotateX (GLfloat* result, GLfloat rotation);
	static void makeRotateY (GLfloat* result, GLfloat rotation);
	static void makeRotateZ (GLfloat* result, GLfloat rotation);

	// Returns a scaling matrix
	static void makeScale (GLfloat* result, GLfloat x, GLfloat y, GLfloat z);

	// Make a perspective camera
	static void makePerspectiveMatrix (GLfloat* result, GLfloat fov, GLfloat aspect, GLfloat nearPlane, GLfloat farPlane);

	// Copies the src matrix into the dest matrix
	static void copyMatrix (GLfloat* src, GLfloat* dest);
	static void print4x4Matrix(GLfloat* mat);
    static void makeOrtho(GLfloat* result, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
	       GLfloat zNear, GLfloat zFar);
// Written by Jeffrey Chastine, 2012


// This is a column-order matrix multiply of matrices m1 and m2.
void matrixMult4x4Column (GLfloat* result, GLfloat* m1, GLfloat* m2)
{
	for (int i = 0; i < 16; i++) {
		result[i] = 0.0f;
	}
	result[T0] = m1[T0]*m2[T0]+m1[T1]*m2[T4]+m1[T2]*m2[T8]+m1[T3]*m2[T12];
	result[T1] = m1[T0]*m2[T1]+m1[T1]*m2[T5]+m1[T2]*m2[T9]+m1[T3]*m2[T13];
	result[T2] = m1[T0]*m2[T2]+m1[T1]*m2[T6]+m1[T2]*m2[T10]+m1[T3]*m2[T14];
	result[T3] = m1[T0]*m2[T3]+m1[T1]*m2[T7]+m1[T2]*m2[T11]+m1[T3]*m2[T15];

	result[T4] = m1[T4]*m2[T0]+m1[T5]*m2[T4]+m1[T6]*m2[T8]+m1[T7]*m2[T12];
	result[T5] = m1[T4]*m2[T1]+m1[T5]*m2[T5]+m1[T6]*m2[T9]+m1[T7]*m2[T13];
	result[T6] = m1[T4]*m2[T2]+m1[T5]*m2[T6]+m1[T6]*m2[T10]+m1[T7]*m2[T14];
	result[T7] = m1[T4]*m2[T3]+m1[T5]*m2[T7]+m1[T6]*m2[T11]+m1[T7]*m2[T15];

	result[T8] = m1[T8]*m2[T0]+m1[T9]*m2[T4]+m1[T10]*m2[T8]+m1[T11]*m2[T12];
	result[T9] = m1[T8]*m2[T1]+m1[T9]*m2[T5]+m1[T10]*m2[T9]+m1[T11]*m2[T13];
	result[T10] = m1[T8]*m2[T2]+m1[T9]*m2[T6]+m1[T10]*m2[T10]+m1[T11]*m2[T14];
	result[T11] = m1[T8]*m2[T3]+m1[T9]*m2[T7]+m1[T10]*m2[T11]+m1[T11]*m2[T15];

	result[T12] = m1[T12]*m2[T0]+m1[T13]*m2[T4]+m1[T14]*m2[T8]+m1[T15]*m2[T12];
	result[T13] = m1[T12]*m2[T1]+m1[T13]*m2[T5]+m1[T14]*m2[T9]+m1[T15]*m2[T13];
	result[T14] = m1[T12]*m2[T2]+m1[T13]*m2[T6]+m1[T14]*m2[T10]+m1[T15]*m2[T14];
	result[T15] = m1[T12]*m2[T3]+m1[T13]*m2[T7]+m1[T14]*m2[T11]+m1[T15]*m2[T15];
}
void matrixMult4x4 (GLfloat* result, GLfloat* m1, GLfloat* m2)
{
	for (int i = 0; i < 16; i++) {
		result[i] = 0.0f;
	}
	result[0] = m1[0]*m2[0]+m1[1]*m2[4]+m1[2]*m2[8]+m1[3]*m2[12];
	result[1] = m1[0]*m2[1]+m1[1]*m2[5]+m1[2]*m2[9]+m1[3]*m2[13];
	result[2] = m1[0]*m2[2]+m1[1]*m2[6]+m1[2]*m2[10]+m1[3]*m2[14];
	result[3] = m1[0]*m2[3]+m1[1]*m2[7]+m1[2]*m2[11]+m1[3]*m2[15];

	result[4] = m1[4]*m2[0]+m1[5]*m2[4]+m1[6]*m2[8]+m1[7]*m2[12];
	result[5] = m1[4]*m2[1]+m1[5]*m2[5]+m1[6]*m2[9]+m1[7]*m2[13];
	result[6] = m1[4]*m2[2]+m1[5]*m2[6]+m1[6]*m2[10]+m1[7]*m2[14];
	result[7] = m1[4]*m2[3]+m1[5]*m2[7]+m1[6]*m2[11]+m1[7]*m2[15];

	result[8] = m1[8]*m2[0]+m1[9]*m2[4]+m1[10]*m2[8]+m1[11]*m2[12];
	result[9] = m1[8]*m2[1]+m1[9]*m2[5]+m1[10]*m2[9]+m1[11]*m2[13];
	result[10] = m1[8]*m2[2]+m1[9]*m2[6]+m1[10]*m2[10]+m1[11]*m2[14];
	result[11] = m1[8]*m2[3]+m1[9]*m2[7]+m1[10]*m2[11]+m1[11]*m2[15];

	result[12] = m1[12]*m2[0]+m1[13]*m2[4]+m1[14]*m2[8]+m1[15]*m2[12];
	result[13] = m1[12]*m2[1]+m1[13]*m2[5]+m1[14]*m2[9]+m1[15]*m2[13];
	result[14] = m1[12]*m2[2]+m1[13]*m2[6]+m1[14]*m2[10]+m1[15]*m2[14];
	result[15] = m1[12]*m2[3]+m1[13]*m2[7]+m1[14]*m2[11]+m1[15]*m2[15];
}


// Makes an identity matrix
void makeIdentity (GLfloat* result)
{
	for (int i = 0; i < 16; i++) {
		result[i] = 0.0f;
	}
	result[0] = result[5] = result[10] = result[15]= 1.0f;
}

// Returns a matrix that translates by x, y and z amount
void makeTranslate (GLfloat* result, GLfloat x, GLfloat y, GLfloat z)
{
	makeIdentity(result);
	result[3] = x;
	result[7] = y;
	result[11] = z;
}

// Returns matrices that rotate about the X, Y and Z axes by a rotation amount (radians)
void makeRotateX (GLfloat* result, GLfloat rotation)
{
	makeIdentity(result);
	result[5] = result[10] = cos(rotation);
	result[6] = sin(rotation);
	result[9] = -result[6];
}
void makeRotateY (GLfloat* result, GLfloat rotation)
{
	makeIdentity(result);
	result[0] = result[10] = cos(rotation);
	result[2] = -sin(rotation);
	result[8] = -result[2];
}
void makeRotateZ (GLfloat* result, GLfloat rotation)
{
	makeIdentity(result);
	result[0] = result[5] = cos(rotation);
	result[1] = sin(rotation);
	result[4] = -result[1];
}

// 2) Create the makeScale method here and fill in the code using the examples above and below
void makeScale(GLfloat* result, GLfloat x, GLfloat y, GLfloat z)
{
	makeIdentity (result);
	result [0] = x;
	result [5] = y;
	result [10] = z;
}


void makePerspectiveMatrix (GLfloat* result, GLfloat fov, GLfloat aspect, GLfloat nearPlane, GLfloat farPlane) {
	GLfloat f = 1.0f/tan(fov*3.1415926f/360.0f);
	makeIdentity(result);
	result[0] = f/aspect;
	result[5] = f;
	result[10] = ((farPlane+nearPlane)/(nearPlane-farPlane));
	result[11] = -1;
	result[14] = (2.0f*farPlane*nearPlane)/(nearPlane-farPlane);
	result[15] = 0;
}

void translateMatrixBy (GLfloat* result, GLfloat x, GLfloat y, GLfloat z)
{
	result[3] += x;
	result[7] += y;
	result[11] += z;
}

void copyMatrix (GLfloat* src, GLfloat* dest)
{
	for (int i = 0; i < 16; i++) {
		dest[i] = src[i];
	}
}

void print4x4Matrix(GLfloat* mat)
{
	printf ("====================================");
	for (int i = 0; i < 16; i++) {
		if (i%4 == 0) printf ("\n");
		printf ("%f\t", mat[i]);
	}
	printf ("\n");
}

void makeOrtho(GLfloat* result, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
	GLfloat zNear, GLfloat zFar)
{
	
	//makeIdentity (result);
	result[0] = 2.0/(right-left);
	result[5] = 2.0/(top-bottom);
    result[10] = 2.0/(zNear-zFar);
    result[15] = 1.0;
    result[12] = -(right+left)/(right-left);
    result[13] = -(top+bottom)/(top-bottom);
    result[14] = -(zFar+zNear)/(zFar - zNear);


}


#endif