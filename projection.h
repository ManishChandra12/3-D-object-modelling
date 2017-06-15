#ifndef PROJECTION_H_INCLUDED
#define PROJECTION_H_INCLUDED

#ifndef _PERSPECTIVE_H_
#define _PERSPECTIVE_H_

#include "Transformation.h"
#include "VertexColorHeader.h"

//changes 3D vertex into corresponding plotable 2D vertex
Vertex3D perspective(const Vertex3D& source, const Vertex3D& cam,
    const Vertex3D& view, float n, float f, int width, int height){

    // For perspective transformation
    float ang = 120; // some view angle
    float ratio = (float)width/height; // ratio of the screen
    float maxDepth = 0x5000;
    float tangent = std::tan( RADIAN(ang/2) );
    Matrix perspective({4,4});
    perspective.init(
            1/tangent,  0,              0,              0,
            0,          ratio/tangent,  0,              0,
            0,          0,              -(n+f)/(f-n),   -(2*f*n)/(f-n),
            0,          0,              -1,              0
            );
    // Note f and n are positive values



    // For device co-ordinate
    Matrix todevice({4,4});
    todevice.init(
            width,      0,          0,              width/2,
            0,          -height,    0,              height/2,
            0,          0,          -0.5*maxDepth,  0.5 * maxDepth,
            0,          0,          0,              1.0
            );
    // here, maxDepth is the maximum value for depth,
    // if not mulitplied then we have a normalized Z value



    // For lookAt matrix


    // I have assumed that "cam" is the camera position
    // and "view" the the point where camera must look
    Vertex3D vrp = cam;
    Vertex3D vup(0,1,0);
    Vertex3D vpn = cam - view;

    Vertex3D nn = vpn.normalized();
    Vertex3D u = (nn.crossProduct(vup)).normalized();
    Vertex3D v = (u.crossProduct(nn)).normalized();

    // UVN system is left handed so forward is negative
    // Translate + Rotate
    Matrix lookAt({4,4});
    lookAt.init(
            u.x,        u.y,        u.z,        u.dotProduct((vrp*-1)),
            v.x,        v.y,        v.z,        v.dotProduct((vrp*-1)),
            nn.x,       nn.y,      nn.z,     nn.dotProduct((vrp*-1)),
            0,          0,          0,          1
            );


    Matrix transformer = todevice * perspective * lookAt;

    Matrix copy({4, 1});
    copy.init(source.x, source.y, source.z, 1);

    copy = transformer*copy;

    Vertex3D res;
    res.x = copy(0) / copy(3);
    res.y = copy(1) / copy(3);
    res.z = copy(2) / copy(3);

    return res;//return 2D equivalent vertex of the 3D source vertex
}

#endif


#endif // PROJECTION_H_INCLUDED
