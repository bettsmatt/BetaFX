/*
 * FileReader.cpp
 *
 *  Created on: Oct 15, 2012
 *      Author: stevananet
 */

#include "FileReader.h"
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "Skeleton.h"
#include "define.h"
#include <math.h>

FileReader::FileReader(bone* r) {
	buffSize = 200;
	maxBones = 60;
	currentFrameNumber = 0;
	numBones = 1;
	root = r;
}

FileReader::~FileReader() {
	// TODO Auto-generated destructor stub
}

void FileReader::readAMC(char* filename){
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("Failed to open file %s\n", filename);
		exit(EXIT_FAILURE);
	}
	char* buff = new char[buffSize];
	/*float x, y, z;
	float tx, ty, tz;*/

	printf("Starting reading motion capture file\n");

	if (fgets(buff, buffSize, file)) {
		while(strchr(buff, '#')){
			fgets(buff, buffSize, file);

			char* temp = buff;
			char head[50];
			char rest[200];
			char name[50];
			char transformations[200];
			float x, y, z = 0;
			float tx, ty, tz = 0;

			sscanf(temp, ":%s %s", head, rest);
			if (strcmp(head, "FULLY-SPECIFIED") == 0) {
				// Skip over :DEGREES
				fgets(buff, buffSize, file);
				while(fgets(buff, buffSize, file) != NULL) {
					sscanf(buff,"%d",&currentFrameNumber);
//					printf("============== BEGIN FRAME %i============\n", currentFrameNumber);
					for (int b = 0; b < NUM_BONES_WITH_DOF; b++) {
						fgets(buff, buffSize, file);
						temp = buff;
						sscanf(temp, "%s %[^\n]", name, transformations);
						for(int i = 0; i < numBones; i++){
							if(strncmp(name, root[i].name, strlen(root[i].name)) == 0){
								int hasx = root[i].dof & DOF_RX;
								int hasy = root[i].dof & DOF_RY;
								int hasz = root[i].dof & DOF_RZ;

								if(hasx && hasy && hasz){
									sscanf(transformations, "%f %f %f", &x, &y, &z);
									bonerotation* br = (bonerotation*) malloc(sizeof(bonerotation));
									br->rx = x; br->ry = y; br->rz = z;
									root[i].frames[currentFrameNumber] = *br;
//									printf("	%s", root[i].name);
//									printf("	DOF: %i, %f %f %f\n", root[i].dof, br->rx, br->ry, br->rz);
									break;
								}
								else if(hasx && hasy){
									sscanf(transformations, "%f %f", &x, &y);
									bonerotation* br = (bonerotation*) malloc(sizeof(bonerotation));
									br->rx = x; br->ry = y;
									root[i].frames[currentFrameNumber] = *br;
//									printf("	%s", root[i].name);
//									printf("	DOF: %i, %f %f\n", root[i].dof, br->rx, br->ry);
									break;
								}
								else if(hasx && hasz){
									sscanf(transformations, "%f %f", &x, &z);
									bonerotation* br = (bonerotation*) malloc(sizeof(bonerotation));
									br->rx = x; br->rz = z;
									root[i].frames[currentFrameNumber] = *br;
//									printf("	%s", root[i].name);
//									printf("	DOF: %i, %f %f\n", root[i].dof, br->rx, br->rz);
									break;
								}
								else if(hasy && hasz){
									sscanf(transformations, "%f %f", &y, &z);
									bonerotation* br = (bonerotation*) malloc(sizeof(bonerotation));
									br->ry = y; br->rz = z;
									root[i].frames[currentFrameNumber] = *br;
//									printf("	%s", root[i].name);
//									printf("	DOF: %i, %f %f\n", root[i].dof, br->ry, br->rz);
									break;
								}
								else if(hasx){
									sscanf(transformations, "%f", &x);
									bonerotation* br = (bonerotation*) malloc(sizeof(bonerotation));
									br->rx = x;
									root[i].frames[currentFrameNumber] = *br;
//									printf("	%s", root[i].name);
//									printf("	DOF: %i, %f\n", root[i].dof, br->rx);
									break;
								}
								else if(hasy){
									sscanf(transformations, "%f", &y);
									bonerotation* br = (bonerotation*) malloc(sizeof(bonerotation));
									br->ry = y;
									root[i].frames[currentFrameNumber] = *br;
//									printf("	%s", root[i].name);
//									printf("	DOF: %i, %f\n", root[i].dof, br->ry);
									break;
								}
								else if(hasz){
									sscanf(transformations, "%f", &z);
									bonerotation* br = (bonerotation*) malloc(sizeof(bonerotation));
									br->rz = z;
									root[i].frames[currentFrameNumber] = *br;
//									printf("	%s", root[i].name);
//									printf("	DOF: %i, %f\n", root[i].dof, br->rz);
									break;
								}
								// It's a root
								else if(root[i].dof == 8){

									sscanf(transformations, "%f %f %f %f %f %f", &tx, &ty, &tz, &x, &y, &z);
									bonerotation* br = (bonerotation*) malloc(sizeof(bonerotation));
									//root->br[currentFrameNumber] = (bonerotation*) malloc(sizeof(bonerotation));
									br->rx = x; br->ry = y; br->rz = z;
									br->tx = tx; br->ty = ty; br->tz = tz;
									root->frames[currentFrameNumber] = *br;
//									printf("	%s", root[i].name);
//									printf("	DOF: %i, %f %f %f %f %f %f\n", root[i].dof, br->tx, br->ty, br->tz, br->rx, br->ry, br->rz);
									break;
								}
							}

						}

					}
//					printf("============== END OF FRAME %i============\n", currentFrameNumber);
				}

			}
		}
	}

	delete[] buff;
	fclose(file);
	printf("Completed reading motion capture file\n");
}


bool FileReader::readASF(char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("Failed to open file %s\n", filename);
		exit(EXIT_FAILURE);
	}

	char* buff = new char[buffSize];
	char *p;

	printf("Starting reading skeleton file\n");
	while ((p = fgets(buff, buffSize, file)) != NULL) {
		//Reading actually happened!

		char* start = buff;
		trim(&start);

		//Check if it is a comment or just empty
		if (buff[0] == '#' || buff[0] == '\0') {
			continue;
		}

		start = strchr(buff, ':');
		if (start != NULL) {
			//This actually is a heading
			//Reading may actually consume the next heading
			//so headings need to be a recursive construct?
			readHeading(buff, file);
		}
	}

	delete[] buff;
	fclose(file);
	printf("Completed reading skeleton file\n");
	return true;
}

/**
 * Trim the current string, by adding a null character into where the comments start
 */
void FileReader::decomment(char * buff) {
	char* comStart = strchr(buff, '#');
	if (comStart != NULL) {
		//remove irrelevant part of string
		*comStart = '\0';
	}
}

void FileReader::readHeading(char* buff, FILE* file) {
	char* temp = buff;
	decomment(buff);
	trim(&temp);

	char head[50];
	char rest[200];
	int num = sscanf(temp, ":%s %s", head, rest);
	if (num == 0) {
		printf("Could not get heading name from %s, all is lost\n", temp);
		exit(EXIT_FAILURE);
	}
	if (strcmp(head, "version") == 0) {
		//version string - must be 1.10
		char* version = rest;
		if (num != 2) {
			char *p = { '\0' };
			while (strlen(p) == 0) {
				char* p = fgets(buff, buffSize, file);
				decomment(p);
				trim(&p);
				version = p;
			}
		}
		if (strcmp(version, "1.10") != 0) {
			printf("Invalid version: %s, must be 1.10\n", version);
			exit(EXIT_FAILURE);
		}
		//Finished reading version so read the next thing?
	} else if (strcmp(head, "name") == 0) {
		//This allows the skeleton to be called something
		//other than the file name
		//We don't actually care what the name is, so can
		//ignore this whole section

	} else if (strcmp(head, "documentation") == 0) {
		//Documentation section has no meaningful information
		//only of use if you want to copy the file. So we skip it
	} else if (strcmp(head, "units") == 0) {
		//Has factors for the units
		//To be able to model the real person,
		//these must be parsed correctly
		//Only really need to check if deg or rad, but even
		//that is probably not needed for the core or extension
	} else if (strcmp(head, "root") == 0) {
		//Read in information about root
		//Or be lazy and just assume it is going to be the normal CMU thing!
	} else if (strcmp(head, "bonedata") == 0) {
		//Description of each bone
		//This does need to actually be read :(
		char *p;
		while ((p = fgets(buff, buffSize, file)) != NULL) {
			decomment(p);
			trim(&p);
			if (strlen(p) > 0) {
				if (p[0] == ':') {
					return readHeading(buff, file);
				} else if (strcmp(p, "begin") != 0) {
					printf("Expected begin for bone data %d, found \"%s\"", numBones, p);
					exit(EXIT_FAILURE);
				} else {
					readBone(buff, file);
					numBones++;
				}

			}
		}
	} else if (strcmp(head, "hierarchy") == 0) {
		//Description of how the bones fit together
		char *p;
		while ((p = fgets(buff, buffSize, file)) != NULL) {
			trim(&p);
			decomment(p);
			if (strlen(p) > 0) {
				if (p[0] == ':') {
					return readHeading(buff, file);
				} else if (strcmp(p, "begin") != 0) {
					printf("Expected begin in hierarchy, found %s", p);
					exit(EXIT_FAILURE);
				} else {
					readHierarchy(buff, file);
				}

			}
		}
	} else {
		printf("Unknown heading %s\n", head);
	}

}

void FileReader::readHierarchy(char* buff, FILE* file) {
	char *p;
	char t1[200];
	while ((p = fgets(buff, buffSize, file)) != NULL) {
		trim(&p);
		decomment(p);
		if (strlen(p) > 0) {
			if (strcmp(p, "end") == 0) {
				//end of hierarchy
				return;
			} else {
				//Read the root node
				sscanf(p, "%s ", t1);
				bone* rootBone = NULL;
				for (int i = 0; i < numBones; i++) {
					if (strcmp(root[i].name, t1) == 0) {
						rootBone = root + i;
						break;
					}
				}
				//Read the connections
				p += strlen(t1);
				bone* other = NULL;
				while (*p != '\0') {
					sscanf(p, "%s ", t1);

					for (int i = 0; i < numBones; i++) {
						if (strcmp(root[i].name, t1) == 0) {
							other = root + i;
							break;
						}
					}
					if (other == NULL) {
						printf("Unknown bone %s found in hierarchy. Failure", t1);
						exit(EXIT_FAILURE);
					}
					rootBone->children[rootBone->numChildren] = other;
					rootBone->numChildren++;
					p += strlen(t1) + 1;

				}
			}
		}

	}
}

void FileReader::readBone(char* buff, FILE* file) {
	char *p;
	char t1[50];
	int id = 0;
	while ((p = fgets(buff, buffSize, file)) != NULL) {
		trim(&p);
		decomment(p);
		if (strlen(p) > 0) {
			if (strcmp(p, "end") == 0) {
				//end of this bone
				return;
			} else {
				sscanf(p, "%s ", t1);
				if (strcmp(t1, "id") == 0) {
					sscanf(p, "id %i", &id);
					root[numBones].id = id;
				}
				else if (strcmp(t1, "name") == 0) {
					//Read the name and actually remember it
					char* name = (char*) malloc(sizeof(char) * 10);
					sscanf(p, "name %s", name);
					root[numBones].name = name;
				} else if (strcmp(t1, "direction") == 0) {
					//Also actually important
					float x, y, z;
					sscanf(p, "direction %f %f %f", &x, &y, &z);
					root[numBones].dirx = x;
					root[numBones].diry = y;
					root[numBones].dirz = z;
				} else if (strcmp(t1, "length") == 0) {
					//Also actually important
					float len;
					sscanf(p, "length %f", &len);
					root[numBones].length = len;
				} else if (strcmp(t1, "dof") == 0) {
					//Read the degrees of freedom!
					char d1[5];
					char d2[5];
					char d3[5];
					int num = sscanf(p, "dof %s %s %s", d1, d2, d3);
					switch (num) {
					DOF dof;
					case 3:
						dof = dofFromString(d3);
						root[numBones].dof = root[numBones].dof | dof;
						//fallthrough!!
						/* no break */
					case 2:
						dof = dofFromString(d2);
						root[numBones].dof = root[numBones].dof | dof;
						//fallthrough!!
						/* no break */
					case 1:
						dof = dofFromString(d1);
						root[numBones].dof = root[numBones].dof | dof;
						break;
					}
				} else if (strcmp(t1, "axis") == 0) {
					//Read the rotation axis
					float x, y, z;
					int num = sscanf(p, "axis %f %f %f XYZ", &x, &y, &z);
					if (num != 3) {
						printf("axis format doesn't match expected\n");
						printf("Expected: axis %%f %%f %%f XYZ\n");
						printf("Got: %s", p);
						exit(EXIT_FAILURE);
					}
					root[numBones].rotx = x;
					root[numBones].roty = y;
					root[numBones].rotz = z;
				}
				//There are more things but they are not needed for the core
			}

		}
	}
}


/**
 * Helper function to turn a DOF from the AMC file into the correct DOF value
 */
DOF FileReader::dofFromString(char* s) {
	if (strcmp(s, "rx") == 0)
		return DOF_RX;
	if (strcmp(s, "ry") == 0)
		return DOF_RY;
	if (strcmp(s, "rz") == 0)
		return DOF_RZ;
	printf("Unknown DOF found: %s", s);
	return DOF_NONE;
}

/*
 * Remove leading and trailing whitespace. Increments the
 * pointer until it points to a non whitespace char
 * and then adds nulls to the end until it has no
 * whitespace on the end
 */
void trim(char **p) {
	if (p == NULL) {
		printf("File terminated without version number!\n");
		exit(EXIT_FAILURE);
	}

	//Remove leading whitespace
	while (**p == ' ' || **p == '\t') {
		(*p)++;
	}

	int len = strlen(*p);
	while (len > 0) {
		char last = (*p)[len - 1];
		if (last == '\r' || last == '\n' || last == ' ' || last == '\t') {
			(*p)[--len] = '\0';
		} else {
			return;
		}
	}
}
