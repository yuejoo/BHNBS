//--------------------------------------------
//GLMATH.h
//For vision math methods
//--------------------------------------------

#ifndef GLMATH_H
#define GLMATH_H

#include "Angel.h"
#define HAFW_WIDTH 512
#define HAFW_HEIGHT 384


//
//--------------------------------------------
// Reproject points in image to the 3D world
//--------------------------------------------
//
inline
vec4 reprojection(const mat4& T, const vec4& points)
{
	vec4 temp = gluInvertMatrix(T)*points;
	return temp/temp[3];
}
//
//--------------------------------------------
// Transfrom pixels to homo coordinate.
//--------------------------------------------
//
inline
vec4 pixels2homo(const int& x, const int& y, const float& width, const float& height, const float& zFar, const float& zNear, const float& estZ) 
{
	if(zFar==zNear)
		return vec4(0,0,0,0);
	return vec4((x-width)/width, -(y-height)/height, (zFar+zNear+2*zFar*zNear/-estZ)/(zFar-zNear),1.0f);
}
//
//--------------------------------------------
// Solve the intersection of the Line and the sphere
//--------------------------------------------
//
inline 
vec4 sol_inter_sphereandline(const vec4& src, const float& radius)
{	
	return vec4(sqrt(radius*radius - (src[1]*src[1]+src[2]*src[2])),src[1],src[2],1.0f);
}

//
//--------------------------------------------
// Compute trangle's normals
//--------------------------------------------
//
inline
vec3 trig_normal(const vec4& a, const vec4& b, const vec4& c)
{
	return normalize( cross(b - a, c - a) );
}




#endif //GLMATH_H
