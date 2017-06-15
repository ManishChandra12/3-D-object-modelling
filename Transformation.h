#ifndef _TRANSFORMATION_H_
#define _TRANSFORMATION_H_

#include "VertexColorHeader.h"

//gives the matrix normalizes the viewpoint and pipelining
//theta = human eyes' viewing angle
//ratio = width/height
//n = near point/plane(XY)
//f = far point/plane(XY)


//gives the matrix that rotates around x-axis
Matrix rotateX(float theta){
	Matrix temp({4,4});
	float cosine = cos(theta);
	float sine = sin(theta);
	temp.init(
		1,      0,      0,      0,
        0,      cosine, -sine,  0,
        0,      sine,   cosine, 0,
        0,      0,      0,      1
		);
	return temp;
}

//gives the matrix that rotates around y-axis
Matrix rotateY(float theta){
	Matrix temp({4,4});
	float cosine = cos(theta);
	float sine = sin(theta);
	temp.init(
	    cosine, 0,      sine,   0,
	    0,      1,      0,      0,
	    -sine,  0,      cosine, 0,
	    0,      0,      0,      1
		);
	return temp;
}

//gives the matrix that rotates around z-axis
Matrix rotateZ(float theta){
	Matrix temp({4,4});
	float cosine = cos(theta);
	float sine = sin(theta);
	temp.init(
		cosine,	-sine,   0,  0,
        sine,	cosine,	0, 	0,
        0,      0,   	1, 	0,
        0,      0,      0,  1
		);
	return temp;
}

//gives the scaling matrix to scale by a factor about origin
Matrix scaling(float s){
	Matrix temp({4,4});
	temp.init(
		s, 0, 0, 0,
		0, s, 0, 0,
		0, 0, s, 0,
		0, 0, 0, 1
		);
	return temp;
}

//gives the translation matrix to translate from a 3D point to origin
Matrix translation(const Vertex3D v){
	Matrix temp({4,4});
	temp.init(
		1, 0, 0, v.x,
		0, 1, 0, v.y,
		0, 0, 1, v.z,
		0, 0, 0, 1
		);
	return temp;
}

#endif
