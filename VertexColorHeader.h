#ifndef VERTEXCOLORHEADER_H_INCLUDED
#define VERTEXCOLORHEADER_H_INCLUDED

#include <cmath>
#include <iostream>
#include <string.h>

#define ABS(a) ((a < 0) ? -a : a) //absolute value
#define DEGREE(a) (a * 180 / PI) //equivalent angle in degree for its radian value
#define EQUAL(a, b) ((bool)(ABS(a - b)) <= PRECISION) //return if a and b are equal or not
#define MAX(a, b) (a > b ? a : b) //maximum of two numbers
#define MIN(a, b) (a < b ? a : b) //minimum of two numbers
#define PI 3.1415926 //value of pi
#define PRECISION 0.000001 //precision required
#define RADIAN(a) (a * PI / 180) //equivalent radian value for provided degree angle
#define ROUNDOFF(a) ((int)((a < 0) ? (a - 0.5) : (a + 0.5))) //gives the nearest integer value of float

//color class
class Color
{
public:
	float r, g, b, a; //variables to hold red, blue, green and alpha component of the color
	Color():r(0xff), g(0xff), b(0xff), a(0xff){} //default is a white
	Color(float red, float green, float blue, float alpha = 0xff):
		r(red), g(green), b(blue), a(alpha){}
	~Color(){}
};

//3D vertex with z coordinate included
class Vertex3D
{
public:
    Vertex3D():x(0), y(0), z(0){}
	Vertex3D(float xx, float yy, float zz):x(xx), y(yy), z(zz){}
	Vertex3D crossProduct (const Vertex3D) const; //a x b
	Vertex3D operator* (const float) const; //a * f
	Vertex3D operator+ (const Vertex3D) const; //a + b
	Vertex3D operator- (const Vertex3D) const; //a - b
	Vertex3D operator/ (const float) const; //a / f
	float cosine(const Vertex3D&) const; //costheta between two vectors
	float dotProduct (const Vertex3D) const; //a . b
	float magnitude() const; //|a|
	Vertex3D normalized();
	void normalize();
	float x, y, z;
	~Vertex3D(){} //destructor
};
Vertex3D Vertex3D::normalized(){
	float m = magnitude();
	if (EQUAL(m, 0.0))
		return {0, 0, 0};
	return {x/m, y/m, z/m};
}
void Vertex3D::normalize(){
	float M = magnitude();
	if (!EQUAL(M, 0.0))
		(*this) = (*this) / M;
}
Vertex3D Vertex3D::crossProduct (const Vertex3D vec) const{
	return Vertex3D(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
}
Vertex3D Vertex3D::operator* (const float f) const{
	return Vertex3D(x*f, y*f, z*f);
}
Vertex3D Vertex3D::operator+ (const Vertex3D vec) const{
	return Vertex3D(x+vec.x, y+vec.y, z+vec.z);
}
Vertex3D Vertex3D::operator- (const Vertex3D vec) const{
	return Vertex3D(x-vec.x, y-vec.y, z-vec.z);
}
Vertex3D Vertex3D::operator/ (const float f) const{
	return Vertex3D(x/f, y/f, z/f);
}
float Vertex3D::cosine(const Vertex3D& v) const{
	return ((*this).dotProduct(v) / ((*this).magnitude() * v.magnitude()));
}
float Vertex3D::dotProduct (const Vertex3D vec) const{
	return (x*vec.x + y*vec.y + z*vec.z);
}
float Vertex3D::magnitude() const{
	return sqrt(x*x + y*y + z*z);
}

class LightSource{
public:
	Color Intensity;
	Vertex3D pos;
	LightSource(Vertex3D v, Color c):pos(v), Intensity(c){}
	~LightSource(){}
};

class ColorVertex
{
public:
	float x, y, z;
	Color col;
	ColorVertex(){}
	ColorVertex(const ColorVertex& c):x(c.x), y(c.y), z(c.z), col(c.col){}
	ColorVertex(float a, float b, float d, Color c):x(a), y(b), z(d), col(c){}
	ColorVertex(Vertex3D v, Color c):x(v.x), y(v.y), z(v.z), col(c){}
	~ColorVertex(){}

};

class Matrix
{
private:
	int row, col;
public:
	float *data;

	//constructor
	Matrix(int r, int c):row(r),col(c){
		data = new float[row * col];
	}
	//copy constructor
	Matrix(const Matrix& m){
		data = new float[m.row*m.col];
		row = m.row;
		col = m.col;
		memcpy(data, m.data, row*col*sizeof(float));
	}
	//destructor
	~Matrix(){
		if(this->data){
			delete []data;
		}
	}
	//matrix initializer, receives any numbers of arguments
	template<typename... Types>
	void init(Types... args){
		const int size = sizeof...(args);
		if(size != row*col)
			return;
		float temp[] = {static_cast<float>(args)...};
		memcpy(data, temp, row*col*sizeof(float));
	}

	Matrix operator* (const Matrix&) const; //returns this * mat (cross-product)
	Matrix operator+ (const Matrix&) const; //returns this + mat
	Matrix operator- (const Matrix&) const; //returns this - mat
	Vertex3D operator* (Vertex3D); //returns (*this) * Vertex3D (a new Vertex3D)
	const float& operator() (int) const; //returns value at Matrix(pos) = Matrix.data(pos)
	const float& operator() (int, int) const; //returns value of Matrix(r, c)
	float& operator() (int); //returns the value of Matrix(pos) = Matrix.data[pos]
	float& operator() (int, int); //returns the value of Matrix(r,c)
	int giveCol(){return col;} //gives the column number
	int giveRow(){return row;} //gives the row number
	void displayMat(); //display matrix in row*col form
	void operator%= (const Matrix&); //returns mat * this (cross product)
	void operator= (const Matrix&); //assignment operator overloaded
};

Matrix Matrix::operator* (const Matrix& mat) const{
	if(col != mat.row){
		std::cout<<"Unequal matrix dimensions. (operator*)\n";
		throw "ERROR";
	}
	Matrix res(row, mat.col);
	for (int i = 0; i < row; i++){
		for (int j = 0; j < mat.col; j++){
			res(i,j) = 0;
			for (int k = 0; k < col; k++){
				int pos = (col)*i + k;
				res(i,j) += data[pos] * mat(k,j);
			}
		}
	}
	return res;
}

Matrix Matrix::operator+ (const Matrix& mat) const{
	if(row != mat.row && col != mat.col){
		std::cout<<"Unequal matrix dimensions. (operator+)\n";
		throw "ERROR";
	}
	Matrix res(row, col);
	for (int i = 0; i < row; i++){
		for (int j = 0; j < col; j++){
			int pos = (col)*i + j;
			res(i,j) = data[pos] + mat(i,j);
		}
	}
	return res;
}

Matrix Matrix::operator- (const Matrix& mat) const{
	if(row != mat.row && col != mat.col){
		std::cout<<"Unequal matrix dimensions. (operator-)\n";
		throw "ERROR";
	}
	Matrix res(row, col);
	for (int i = 0; i < row; i++){
		for (int j = 0; j < col; j++){
			int pos = (col)*i + j;
			res(i,j) = data[pos] - mat(i,j);
		}
	}
	return res;
}

Vertex3D Matrix::operator*(Vertex3D v){
	Matrix temp({4,1});
	temp.init(v.x, v.y, v.z, 1);
	temp = (*this) * temp;
	return Vertex3D(temp(0), temp(1), temp(2));
}

const float& Matrix::operator() (int pos) const {
	return data[pos];
}

const float& Matrix::operator() (int r, int c) const {
	int pos = col*r + c;
	return data[pos];
}

float& Matrix::operator() (int pos){
	return data[pos];
}

float& Matrix::operator() (int r, int c){
	int pos = col*r + c;
	return data[pos];
}

void Matrix::displayMat(){
	for (int i = 0; i < row; i++){
		for (int j = 0; j < col; j++){
			int pos = col*i + j;
			std::cout<<data[pos]<<" ";
		}
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
}

void Matrix::operator%= (const Matrix& mat){
	Matrix temp = mat*(*this);
	*this = temp;
}

void Matrix::operator= (const Matrix& mat){
	if(this == &mat)
		return;
	if(row*col != mat.row*mat.col){
		delete []data;
		data = new float[mat.row*mat.col];
	}
	row = mat.row;
	col = mat.col;
	memcpy(data, mat.data, row*col*sizeof(float));
}


#endif // VERTEXCOLORHEADER_H_INCLUDED
