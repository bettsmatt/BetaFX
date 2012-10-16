/*
 * FileReader.h
 *
 *  Created on: Oct 15, 2012
 *      Author: stevananet
 */

#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <stdio.h>
#include <GL/glut.h>
#include <GL/glut.h>
#include "Skeleton.h"

typedef int DOF;

#define NUM_FRAMES 600;
#define NUM_BONES_WITH_DOF 29
#define DOF_NONE 0
#define DOF_RX 1
#define DOF_RY 2
#define DOF_RZ 4
#define DOF_ROOT 8 // Root has 6, 3 translation and 3 rotation

void trim(char**);

class FileReader {

private:
	int buffSize, maxBones;

public:
	int numBones;
	int totalFrameNum;
	bone* root;
	FileReader(bone*);
	virtual ~FileReader();

	void readAMC(char* filename);
	bool readASF(char* filename);
	void decomment(char * buff);
	void readHeading(char* buff, FILE* file);
	void readHierarchy(char* buff, FILE* file);
	void readBone(char* buff, FILE* file);
	DOF dofFromString(char* s);
};

#endif /* FILEREADER_H_ */
