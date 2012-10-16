//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Matthew Betts
//
// Written by Matthew Betts
//
// This software is provided 'as-is' for the BetaFX C308 project
// in ECS, Victoria University of Wellington,
// without any express or implied warranty.
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------


/*
 * Since we have to turn of the depth buffer to render the alpha in particles, we need to order everything before we draw it.
 */
#pragma once

#include "define.h"
#include <GL/glut.h>
#include <queue>

class Render
{

	public:
		Render() {}
		~Render() {}
		virtual void RenderMe() = 0;
		virtual float Dist(float*) = 0;
};


