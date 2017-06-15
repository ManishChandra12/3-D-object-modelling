#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "projection.h"
#include "Screen.h"
#include "Transformation.h"
#include "VertexColorHeader.h"
#include <SDL.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

class RenderObject
{
private:
	std::vector<Vertex3D> avgVerNormal;
	std::vector<Vertex3D> surfaceNormal;
	std::vector<Vertex3D> surfaceTexture;
	std::vector<Vertex3D> surfaceVertex;
	std::vector<Vertex3D> vertexMatrix;
	std::vector<Vertex3D> vertexNormal;
	std::vector<Vertex3D> vertexTexture;
public:
	RenderObject(const string&);
	bool isInsideTriangle(const Vertex3D&, const Vertex3D&, const Vertex3D&, const Vertex3D&);
	void gouraudFill(int, int, Vertex3D&, Vertex3D&,LightSource&);
	void initVertexNormal();
	void rotate(float, float, float,LightSource&);
	void scale(float);
	void translate(Vertex3D);
	void sortVertices(ColorVertex&, ColorVertex&, ColorVertex&, ColorVertex&, ColorVertex&, ColorVertex&);
	~RenderObject(){}
};

void RenderObject::initVertexNormal(){
	for (int i = 0; i < vertexMatrix.size(); i++)
		avgVerNormal.push_back({0, 0, 0});

	for (int i = 0; i < surfaceVertex.size(); i++){
		int iVertex = surfaceVertex[i].x - 1;
		int iNormal = surfaceNormal[i].x - 1;
		Vertex3D normal = vertexNormal[iNormal];
		avgVerNormal[iVertex] = avgVerNormal[iVertex] + normal;

		iVertex = surfaceVertex[i].y - 1;
		iNormal = surfaceNormal[i].y - 1;
		normal = vertexNormal[iNormal];
		avgVerNormal[iVertex] = avgVerNormal[iVertex] + normal;

		iVertex = surfaceVertex[i].z - 1;
		iNormal = surfaceNormal[i].z - 1;
		normal = vertexNormal[iNormal];
		avgVerNormal[iVertex] = avgVerNormal[iVertex] + normal;
	}

	for (int i = 0; i < vertexMatrix.size(); i++)
		(avgVerNormal[i]/3).normalize();
}

void RenderObject::sortVertices(ColorVertex& a, ColorVertex& b, ColorVertex& c, ColorVertex& xx, ColorVertex& yy, ColorVertex& zz){
	if(xx.y <= yy.y && xx.y <= zz.y){
		a = xx;
		if(yy.y <= zz.y){	b = yy; c = zz;		}
		else{	b = zz; c = yy;		}
	}
	else if(yy.y <= xx.y && yy.y <= zz.y){
		a = yy;
		if(xx.y <= zz.y){	b = xx; c = zz;		}
		else{	b = zz; c = xx;		}
	}
	else{
		a = zz;
		if(xx.y <= yy.y){	b = xx; c = yy;		}
		else{	b = yy; c = xx;		}
	}
}

bool RenderObject::isInsideTriangle(const Vertex3D& p, const Vertex3D& a, const Vertex3D& b, const Vertex3D& c){
	float x, y, z;
	x = (p-a).x*(b-a).y - (p-a).y*(b-a).x;
	y = (p-b).x*(c-b).y - (p-b).y*(c-b).x;
	z = (p-c).x*(a-c).y - (p-c).y*(a-c).x;
	return (x >= 0 && y >= 0 && z >= 0) || (x < 0 && y < 0 && z < 0);
}

void RenderObject::rotate(float alpha, float beta, float gamma,LightSource& light){
	Matrix RinX = rotateX(alpha);
	Matrix RinY = rotateY(beta);
	Matrix RinZ = rotateZ(gamma);
	Matrix temp = RinZ * RinY * RinX;
	for (int i = 0; i < vertexMatrix.size(); i++)
		vertexMatrix[i] = temp * vertexMatrix[i];
	for (int i = 0; i < vertexNormal.size(); i++)
		vertexNormal[i] = temp * vertexNormal[i];
    light.pos=temp * light.pos;
}

void RenderObject::scale(float sf){
    Matrix temp=scaling(sf);
    for (int i = 0; i < vertexMatrix.size(); i++)
		vertexMatrix[i] = temp * vertexMatrix[i];
}

void RenderObject::translate(Vertex3D vd){
    Matrix temp=translation(vd);
    for (int i = 0; i < vertexMatrix.size(); i++)
		vertexMatrix[i] = temp * vertexMatrix[i];
}

RenderObject::RenderObject(const string& filename){
	vertexMatrix.clear();
	ifstream objFile(filename.c_str());
	if(!objFile.is_open()) {
		std::cout<<"Can't open the file.\n";
		throw "Can't open";
	}
	std::vector<Vertex3D> temp;
	string line, keyword;
	unsigned int vN = 0, vtN = 0, fN = 0, vnN = 0;
	while(getline(objFile, line)){
		istringstream linestream(line);
		if(line.length() == 0) continue; //if line is empty, reset the keyword
		//this is to be done as the keyword remains same of the latest line and causes errors
		if(line.length() > 0)
			line = line.substr(1, line.length()-1);
		while(line.compare(0,1," ")==0)
			line.erase(line.begin()); //removes leading spaces
		while(line.size() > 0 && line.compare(line.size()-1,1," ")==0)
			line.erase(line.end()-1); //removes trailing spaces
		linestream >> keyword; //get the details keyword
		if(keyword == "v"){
			Vertex3D tempV;
			linestream >> tempV.x;	linestream >> tempV.y;	linestream >> tempV.z;
			vertexMatrix.push_back(tempV); //add new vertex to its vector
			vN++;
		}
		else if(keyword == "vn"){
			Vertex3D tempV;
			linestream >> tempV.x;	linestream >> tempV.y;	linestream >> tempV.z;
			vertexNormal.push_back(tempV); //add new vertex normal to its vector
			vnN++;
		}
		else if(keyword == "vt"){
			Vertex3D tempV;
			linestream >> tempV.x;	linestream >> tempV.y;	linestream >> tempV.z;
			vertexTexture.push_back(tempV); //add new vertex texture to its vector
			vtN++;
		}
		else if(keyword == "f"){
			replaceAll(line,"/"," "); //replaces '/' with <space>
			istringstream lstream(line);
			Vertex3D ver, tex, nor;
			lstream >> ver.x;	lstream >> tex.x;	lstream >> nor.x;
			lstream >> ver.y;	lstream >> tex.y;	lstream >> nor.y;
			lstream >> ver.z;	lstream >> tex.z;	lstream >> nor.z;
			surfaceVertex.push_back(ver); surfaceTexture.push_back(tex); surfaceNormal.push_back(nor);//add new surface to the surface vector
			fN++;
		}
	}
	initVertexNormal();
}

void RenderObject::gouraudFill(int width, int height, Vertex3D& cam, Vertex3D& viewPlane,LightSource& light){

    SDL_WM_SetCaption("Cricket Pitch", NULL);
    Screen Pitch(width, height);
    Pitch.clear();

    Color ia(0.3,0.3,0.3), ks(0.1, 0.1, 0.1), kd(0.5, 0.5, 0.5), ka(0.5, 0.5, 0.5);
	std::vector<LightSource> lighta;
	lighta.push_back(light);

    Color ColorIntensity[vertexMatrix.size()];
    for(unsigned int ii = 0; ii < vertexMatrix.size(); ii++){
    	//get three vertices of the surface

		Vertex3D n = avgVerNormal[ii];
		Vertex3D centroid = vertexMatrix[ii];

		float intensityR = ia.r*ka.r, intensityG = ia.g*ka.g, intensityB = ia.b*ka.b;
		for(int i = 0; i < lighta.size(); i++){

			float costheta = (lighta[i].pos).cosine(n);
			if(costheta > 0){
                    if(ii<8){
				intensityR += lighta[i].Intensity.r*kd.r*costheta*0;
				intensityG += lighta[i].Intensity.g*kd.g*costheta+1;
				intensityB += lighta[i].Intensity.b*kd.b*costheta*0;}

			else if (ii>=8 && ii<390)
            {
                intensityR += lighta[i].Intensity.r*kd.r*costheta+1;
				intensityG += lighta[i].Intensity.g*kd.g*costheta*0;
				intensityB += lighta[i].Intensity.b*kd.b*costheta*0;}

            else
            {
                intensityR += lighta[i].Intensity.r*kd.r*costheta*0;
				intensityG += lighta[i].Intensity.g*kd.g*costheta*0;
				intensityB += lighta[i].Intensity.b*kd.b*costheta+1;}
            }
		}
		ColorIntensity[ii] = Color(intensityR, intensityG, intensityB);
	}

    float near = 5, far = 0xffffff;
    Vertex3D v3[vertexMatrix.size()], lightView[vertexMatrix.size()];
    for(int i = 0; i < vertexMatrix.size(); i++){
        v3[i] = perspective(vertexMatrix[i], cam, viewPlane, near, far, width, height); //conversion to device coordinate
    }
    for(unsigned int i = 0; i < surfaceVertex.size(); i++){

    	//get three vertices of the surface
    	unsigned int x = (unsigned int) surfaceVertex[i].x - 1;
    	unsigned int y = (unsigned int) surfaceVertex[i].y - 1;
    	unsigned int z = (unsigned int) surfaceVertex[i].z - 1;
		ColorVertex a(v3[x], ColorIntensity[x]);
		ColorVertex b(v3[y], ColorIntensity[y]);
		ColorVertex c(v3[z], ColorIntensity[z]);

		Vertex3D n = (v3[y] - v3[x]).crossProduct(v3[z] - v3[y])*-1;
		float d = -(a.x*n.x + a.y*n.y + a.z*n.z);

		ColorVertex A, B, C;
		sortVertices(A, B, C, a, b, c);

		if (A.y == C.y) continue;
		if (A.y >= height || C.y < 0) continue;

		float dx1, dx2, dx3;
		float dr1, dr2, dr3;
		float dg1, dg2, dg3;
		float db1, db2, db3;
		float dr, dg, db;

		if (B.y > A.y){
			dx1 = (B.x - A.x) / (B.y - A.y);
			dr1 = (B.col.r - A.col.r) / (B.y - A.y);
			dg1 = (B.col.g - A.col.g) / (B.y - A.y);
			db1 = (B.col.b - A.col.b) / (B.y - A.y);
		}else dx1 = dr1 = dg1 = db1 = 0;

		if (C.y > A.y){
			dx2 = (C.x - A.x) / (C.y - A.y);
			dr2 = (C.col.r - A.col.r) / (C.y - A.y);
			dg2 = (C.col.g - A.col.g) / (C.y - A.y);
			db2 = (C.col.b - A.col.b) / (C.y - A.y);
		}else dx2 = dr2 = dg2 = db2 = 0;

		if (C.y > B.y){
			dx3 = (C.x - B.x) / (C.y - B.y);
			dr3 = (C.col.r - B.col.r) / (C.y - B.y);
			dg3 = (C.col.g - B.col.g) / (C.y - B.y);
			db3 = (C.col.b - B.col.b) / (C.y - B.y);
		}else dx3 = dr3 = dg3 = db3 = 0;

		ColorVertex S = A;
		ColorVertex E = A;

		if (dx1 > dx2){
			for(; S.y <= B.y; S.y++, E.y++){
				if(E.x > S.x){
					dr = (E.col.r - S.col.r) / (E.x - S.x);
					dg = (E.col.g - S.col.g) / (E.x - S.x);
					db = (E.col.b - S.col.b) / (E.x - S.x);
				}else dr = dg = db = 0;

				ColorVertex P = S;
				for (; P.x < E.x; P.x++){
					float depth = -(n.x*P.x + n.y*P.y + d) / n.z;
					Pitch.setPixel(P.x, P.y, -depth, P.col);
					P.col.r += dr; P.col.g += dg;  P.col.b += db;
				}
				S.x += dx2; S.col.r += dr2; S.col.g += dg2; S.col.b += db2;
				E.x += dx1; E.col.r += dr1; E.col.g += dg1; E.col.b += db1;
			}

			E = B;
			for(; S.y <= C.y; S.y++, E.y++){
				if(E.x > S.x){
					dr = (E.col.r - S.col.r) / (E.x - S.x);
					dg = (E.col.g - S.col.g) / (E.x - S.x);
					db = (E.col.b - S.col.b) / (E.x - S.x);
				}else dr = dg = db = 0;

				ColorVertex P = S;
				for(; P.x < E.x; P.x++){
					float depth = -(n.x*P.x + n.y*P.y + d) / n.z;
					Pitch.setPixel(P.x, P.y, -depth, P.col);
					P.col.r += dr; P.col.g += dg;  P.col.b += db;
				}
				S.x += dx2; S.col.r += dr2; S.col.g += dg2; S.col.b += db2;
				E.x += dx3; E.col.r += dr3; E.col.g += dg3; E.col.b += db3;
			}
		}
		else{
			for(; S.y <= B.y; S.y++, E.y++){
				if(E.x > S.x){
					dr = (E.col.r - S.col.r) / (E.x - S.x);
					dg = (E.col.g - S.col.g) / (E.x - S.x);
					db = (E.col.b - S.col.b) / (E.x - S.x);
				}else dr = dg = db = 0;

				ColorVertex P = S;
				for (; P.x < E.x; P.x++){
					float depth = -(n.x*P.x + n.y*P.y + d) / n.z;
					Pitch.setPixel(P.x, P.y, -depth, P.col);
					P.col.r += dr; P.col.g += dg;  P.col.b += db;
				}
				S.x += dx1; S.col.r += dr1; S.col.g += dg1; S.col.b += db1;
				E.x += dx2; E.col.r += dr2; E.col.g += dg2; E.col.b += db2;
			}

			S = B;
			for(; S.y <= C.y; S.y++, E.y++){
				if(E.x > S.x){
					dr = (E.col.r - S.col.r) / (E.x - S.x);
					dg = (E.col.g - S.col.g) / (E.x - S.x);
					db = (E.col.b - S.col.b) / (E.x - S.x);
				}else dr = dg = db = 0;

				ColorVertex P = S;
				// std::cout<<P.x<<"\t"<<S.x<<"\t"<<E.x<<"\n";
				for(; P.x < E.x; P.x++){
					float depth = -(n.x*P.x + n.y*P.y + d) / n.z;
					Pitch.setPixel(P.x, P.y, -depth, P.col);
					P.col.r += dr; P.col.g += dg;  P.col.b += db;
				}
				S.x += dx3; S.col.r += dr3; S.col.g += dg3; S.col.b += db3;
				E.x += dx2; E.col.r += dr2; E.col.g += dg2; E.col.b += db2;
			}
		}
	}

    Pitch.refresh();
}

#endif
