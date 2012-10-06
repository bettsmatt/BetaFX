/*
 * 308_ImageLoader.h
 *
 *  Created on: Sep 17, 2012
 *      Author: atkinsdani1
 */

#pragma once

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>
#include <png.h>

//Magic defines for libPNG because they removed them from the library
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#define png_voidp_NULL NULL

/*
 *  The TextureInfo struct. This is where we keep all the texture info when we load a file.
 *  The actual image is stored as an array of unsigned characters (aka unsigned bytes).
 *  It's three bytes per pixel -- one for each color channel, so this array will have a length
 *  of width*height*3. Note that it is ONE dimensional!
 */
typedef struct texInfo {

	char *filename;
	unsigned char* textureData;
	int format;
	int width;
	int height;
	bool hasAlpha;

} TextureInfo;

int loadTextureFromJPEG(char*,TextureInfo*);

int loadTextureFromPNG(char*, TextureInfo*);
