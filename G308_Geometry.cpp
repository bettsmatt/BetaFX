//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Taehyun Rhee
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

#include "G308_Geometry.h"
#include "G308_ImageLoader.h"
#include <stdio.h>
#include <math.h>

G308_Geometry::G308_Geometry(void) {
	m_pVertexArray = NULL;
	m_pNormalArray = NULL;
	m_pUVArray = NULL;
	m_pTriangles = NULL;

	mode = G308_SHADE_POLYGON;

	m_nNumPoint = m_nNumUV = m_nNumPolygon = 0;
	m_glGeomListPoly = m_glGeomListWire = 0;

	pos = (v3*) malloc(sizeof (v3));
	pos->x = 0; pos->y = 0; pos->z = 0;

	hasTexture = false;
	textureID = 0;

	hasCubemap = false;

	mat_ambient = (GLfloat*) malloc (sizeof (GLfloat) * 4);
	mat_diffuse = (GLfloat*) malloc (sizeof (GLfloat) * 4);
	mat_specular = (GLfloat*) malloc (sizeof (GLfloat) * 4);
	mat_shininess = (GLfloat*) malloc (sizeof (GLfloat) * 1);


	for(int i = 0 ; i < 4 ; i ++){
		mat_ambient[i] = 1.0f;
		mat_diffuse[i] = 1.0f;
		mat_specular[i] = 1.0f;
	}

	mat_shininess[0] = 1.0f;

	worldRot = 0;

}

G308_Geometry::~G308_Geometry(void) {
	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	if (m_pTriangles != NULL)
		delete[] m_pTriangles;

}

void G308_Geometry::SetCubemap(){
	hasCubemap = true;

}

void G308_Geometry::setTexture(GLuint textureID){
	this->textureID = textureID;
	hasTexture = true;
}

void G308_Geometry::setMaterial(GLfloat* ambient, GLfloat* diffuse, GLfloat* specular, GLfloat* shininess){

	for(int i = 0 ; i < 4 ; i ++){
		mat_diffuse[i] = diffuse[i];
		mat_specular[i] = specular[i];
		mat_ambient[i] = ambient[i];
	}
	mat_shininess[0] = shininess[0];

}

// Set the position
void G308_Geometry::SetPos(v3* newPosition){
	pos = newPosition;
}

//-------------------------------------------------------
// Read in the OBJ (Note: fails quietly, so take care)
//--------------------------------------------------------
void G308_Geometry::ReadOBJ(const char *filename) {
	FILE* fp;
	char mode, vmode;
	char str[200];
	int v1, v2, v3, n1, n2, n3, t1, t2, t3;
	int numVert, numNorm, numUV, numFace;
	float x, y, z;
	float u, v;

	numVert = numNorm = numUV = numFace = 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
		printf("Error reading %s file\n", filename);
	else
		printf("Reading %s file\n", filename);

	// Check number of vertex, normal, uvCoord, and Face
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
				numUV++;
			else if (vmode == 'n') // normal
				numNorm++;
			else if (vmode == ' ') // vertex
				numVert++;
			break;
		case 'f': /* faces */
			numFace++;
			break;
		}
	}

	m_nNumPoint = numVert;
	m_nNumUV = numUV;
	m_nNumPolygon = numFace;
	m_nNumNormal = numNorm;

	printf("Number of Point %d, UV %d, Normal %d, Face %d\n", numVert, numUV,
			numNorm, numFace);
	//-------------------------------------------------------------
	//	Allocate memory for array
	//-------------------------------------------------------------

	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	m_pVertexArray = new G308_Point[m_nNumPoint];

	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	m_pNormalArray = new G308_Normal[m_nNumNormal];

	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	m_pUVArray = new G308_UVcoord[m_nNumUV];

	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
	m_pTriangles = new G308_Triangle[m_nNumPolygon];

	//-----------------------------------------------------------
	//	Read obj file
	//-----------------------------------------------------------
	numVert = numNorm = numUV = numFace = 0;

	fseek(fp, 0L, SEEK_SET);
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
			{
				sscanf(str, "vt %f %f", &u, &v);
				m_pUVArray[numUV].u = u;
				m_pUVArray[numUV].v = v;
				numUV++;
			} else if (vmode == 'n') // normal
			{
				sscanf(str, "vn %f %f %f", &x, &y, &z);
				m_pNormalArray[numNorm].x = x;
				m_pNormalArray[numNorm].y = y;
				m_pNormalArray[numNorm].z = z;
				numNorm++;
			} else if (vmode == ' ') // vertex
			{
				sscanf(str, "v %f %f %f", &x, &y, &z);
				m_pVertexArray[numVert].x = x;
				m_pVertexArray[numVert].y = y;
				m_pVertexArray[numVert].z = z;
				numVert++;
			}
			break;
		case 'f': /* faces : stored value is index - 1 since our index starts from 0, but obj starts from 1 */
			if (numNorm > 0 && numUV > 0) {
				sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
			} else if(numNorm > 0 && numUV ==0){
				sscanf(str, "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
			} else if(numUV > 0 && numNorm==0){
				sscanf(str, "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3);
			} else if(numUV==0 && numNorm==0){
				sscanf(str, "f %d %d %d", &v1, &v2, &v3);
			}
			// Vertex indicies for triangle:
			if (numVert != 0) {
				m_pTriangles[numFace].v1 = v1 - 1;
				m_pTriangles[numFace].v2 = v2 - 1;
				m_pTriangles[numFace].v3 = v3 - 1;
			}

			// Normal indicies for triangle
			if (numNorm != 0) {
				m_pTriangles[numFace].n1 = n1 - 1;
				m_pTriangles[numFace].n2 = n2 - 1;
				m_pTriangles[numFace].n3 = n3 - 1;
			}

			// UV indicies for triangle
			if (numUV != 0) {
				m_pTriangles[numFace].t1 = t1 - 1;
				m_pTriangles[numFace].t2 = t2 - 1;
				m_pTriangles[numFace].t3 = t3 - 1;
			}

			numFace++;
			break;
		default:
			break;
		}
	}

	fclose(fp);
	printf("Reading OBJ file is DONE.\n");

}

//--------------------------------------------------------------
// [Assignment5]
// Create a 2D GL texture from the file given
//--------------------------------------------------------------
void G308_Geometry::ReadTexture(const char* filename) {
	//Your code here
}


void G308_Geometry::EnableCubemap(){




	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glEnable(GL_TEXTURE_CUBE_MAP_EXT);

}

void G308_Geometry::DisableCubemap(){

	glDisable(GL_TEXTURE_CUBE_MAP_EXT);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);

}

void G308_Geometry::CreateGLPolyGeometry() {
	if (m_glGeomListPoly != 0)
		glDeleteLists(m_glGeomListPoly, 1);

	// Assign a display list; return 0 if err
	m_glGeomListPoly = glGenLists(1);
	glNewList(m_glGeomListPoly, GL_COMPILE);

	glBegin(GL_TRIANGLES);

	for(int i = 0 ; i < m_nNumPolygon ; i ++){

		G308_Point v1 = m_pVertexArray[m_pTriangles[i].v1];
		G308_Point v2 = m_pVertexArray[m_pTriangles[i].v2];
		G308_Point v3 = m_pVertexArray[m_pTriangles[i].v3];

		G308_Point n1 = m_pNormalArray[m_pTriangles[i].n1];
		G308_Point n2 = m_pNormalArray[m_pTriangles[i].n2];
		G308_Point n3 = m_pNormalArray[m_pTriangles[i].n3];

		G308_UVcoord t1 = m_pUVArray[m_pTriangles[i].t1];
		G308_UVcoord t2 = m_pUVArray[m_pTriangles[i].t2];
		G308_UVcoord t3 = m_pUVArray[m_pTriangles[i].t3];

		float textMult = 10.0f;

		glTexCoord2f(t1.u * textMult, t1.v * textMult);
		glNormal3f(n1.x, n1.y, n1.z );
		glVertex3f(v1.x, v1.y, v1.z );

		glTexCoord2f(t2.u * textMult, t2.v * textMult);
		glNormal3f(n2.x, n2.y, n2.z );
		glVertex3f(v2.x, v2.y, v2.z );

		glTexCoord2f(t3.u * textMult, t3.v *textMult);
		glNormal3f(n3.x, n3.y, n3.z );
		glVertex3f(v3.x, v3.y, v3.z );

	}

	glEnd();


	glEndList();
}

void G308_Geometry::CreateGLWireGeometry() {
	if (m_glGeomListWire != 0)
		glDeleteLists(m_glGeomListWire, 1);http://gm/

		// Assign a display list; return 0 if err
		m_glGeomListWire = glGenLists(1);
		glNewList(m_glGeomListWire, GL_COMPILE);

		//----------------------
		//YOUR CODE GOES HERE
		//
		// .....
		//
		glBegin(GL_LINE_LOOP);

		// Wireframe
		for(int i = 0 ; i < m_nNumPolygon ; i ++){


			G308_Point v1 = m_pVertexArray[m_pTriangles[i].v1];
			G308_Point v2 = m_pVertexArray[m_pTriangles[i].v2];
			G308_Point v3 = m_pVertexArray[m_pTriangles[i].v3];


			glVertex3f(
					v1.x,
					v1.y,
					v1.z
			);

			glVertex3f(
					v2.x,
					v2.y,
					v2.z
			);

			glVertex3f(
					v3.x,
					v3.y,
					v3.z
			);

		}

		glEnd();

		glEndList();

}

//--------------------------------------------------------------
// [Assignment5]
// Fill the following function to create display list
// of the obj file to show it as polygon, using texture and normal information (if any)
//--------------------------------------------------------------
/*
void G308_Geometry::CreateGLPolyGeometry() {
	if (m_glGeomListPoly != 0)
		glDeleteLists(m_glGeomListPoly, 1);

	// Assign a display list; return 0 if err
	m_glGeomListPoly = glGenLists(1);
	glNewList(m_glGeomListPoly, GL_COMPILE);

	//Your code here

	glEndList();
}/
/*
void G308_Geometry::CreateGLWireGeometry() {
	if (m_glGeomListWire != 0)
		glDeleteLists(m_glGeomListWire, 1);

	// Assign a display list; return 0 if err
	m_glGeomListWire = glGenLists(1);
	glNewList(m_glGeomListWire, GL_COMPILE);

	//Your code here: may not be required for assignment 5

	glEndList();

}
 */

void G308_Geometry::toggleMode() {
	if (mode == G308_SHADE_POLYGON) {
		mode = G308_SHADE_WIREFRAME;
	} else {
		mode = G308_SHADE_POLYGON;
	}
}

void G308_Geometry::RenderGeometry() {

	glPushMatrix();

	glRotatef(worldRot,0,1,0);

	glTranslated(pos->x,pos->y,pos->z);
	/*
	 * Materials
	 */
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);

	if(hasTexture){
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	}

	if(hasCubemap){
		EnableCubemap();
	}


	glBegin(GL_TRIANGLES);

	for(int i = 0 ; i < m_nNumPolygon ; i ++){

		G308_Point v1 = m_pVertexArray[m_pTriangles[i].v1];
		G308_Point v2 = m_pVertexArray[m_pTriangles[i].v2];
		G308_Point v3 = m_pVertexArray[m_pTriangles[i].v3];

		G308_Point n1 = m_pNormalArray[m_pTriangles[i].n1];
		G308_Point n2 = m_pNormalArray[m_pTriangles[i].n2];
		G308_Point n3 = m_pNormalArray[m_pTriangles[i].n3];

		G308_UVcoord t1 = m_pUVArray[m_pTriangles[i].t1];
		G308_UVcoord t2 = m_pUVArray[m_pTriangles[i].t2];
		G308_UVcoord t3 = m_pUVArray[m_pTriangles[i].t3];

		float textMult = 10.0f;

		glTexCoord2f(t1.u * textMult, t1.v * textMult);
		glNormal3f(n1.x, n1.y, n1.z );
		glVertex3f(v1.x, v1.y, v1.z );

		glTexCoord2f(t2.u * textMult, t2.v * textMult);
		glNormal3f(n2.x, n2.y, n2.z );
		glVertex3f(v2.x, v2.y, v2.z );

		glTexCoord2f(t3.u * textMult, t3.v *textMult);
		glNormal3f(n3.x, n3.y, n3.z );
		glVertex3f(v3.x, v3.y, v3.z );

	}


	glEnd();

	if(hasCubemap){
		DisableCubemap();
	}

	if(hasTexture){
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	}




	glPopMatrix();

}
