//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Taehyun Rhee
//
// Edited by Daniel Atkins
//
// This software is provided 'as-is' for assignment of COMP308 
// in ECS, Victoria University of Wellington, 
// without any express or implied warranty. 
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#pragma once

#include "define.h"
#include <GL/glut.h>

typedef struct v3 {
	double x;
	double y;
	double z;
} v3;

class G308_Geometry
{
private:
	// Array for Geometry
	G308_Point* m_pVertexArray;		// Vertex Array
	G308_Normal* m_pNormalArray;	// Normal Array
	G308_Triangle* m_pTriangles;	// Triangle Array
	G308_UVcoord* m_pUVArray;	    // Texture Coordinate Array

	// Data for Geoemetry
	int m_nNumPoint;
	int m_nNumUV;
	int m_nNumNormal;
	int m_nNumPolygon;

	int mode; // Which mode to display

	// Data for Rendering
	int m_glGeomListPoly;	// Display List for Polygon
	int m_glGeomListWire;	// Display List for Wireframe



	bool hasTexture;
	GLuint textureID;

	bool hasCubemap;

	GLfloat* mat_specular;
	GLfloat* mat_shininess;
	GLfloat* mat_diffuse;
	GLfloat* mat_ambient;

public:

	float worldRot;
	v3* pos;

	G308_Geometry(void);
	~G308_Geometry(void);

	void ReadOBJ(const char* filename);
	void ReadTexture(const char* filename);

	void CreateGLPolyGeometry(); // [Assignment5] Create GL Display List for Polygon Geometry, using textures!
	void CreateGLWireGeometry(); // Already written for you, this time.

	void RenderGeometry();     // mode : G308_SHADE_POLYGON, G308_SHADE_WIREFRAME
	void toggleMode(); //Switch between showing filled polygons and wireframes

	void SetPos(v3* newPosition); // Set the position
	void setTexture (GLuint); // Set the Texture
	void setMaterial(GLfloat*, GLfloat*, GLfloat*, GLfloat*); // Material

	void SetCubemap (); // Set cubema

	void EnableCubemap (); // Enable cubemap
	void DisableCubemap (); // Disable cubemap
};
