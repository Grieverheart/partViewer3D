// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2011, Florian Kirsch,
// Hasso-Plattner-Institute at the University of Potsdam, Germany
//
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License, 
// Version 2, as published by the Free Software Foundation.
// As a special exception, you have permission to link this library
// with the CGAL library and distribute executables.
//
// This library is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

//
// displaylistPrimitive.h 
//
// example for a primitive which renders itself using a display list
//

#include <GL/glew.h>
#include <GL/freeglut.h>
typedef unsigned int uint;
#include "../include/displaylistPrimitive.h"

static std::vector<OpenCSG::Primitive*> collection;
static GLuint boxDL;
static float init_boxSize;

namespace OpenCSG {

    DisplayListPrimitive::DisplayListPrimitive(unsigned int i, Operation o, unsigned int c, const float* position, const float* rotation)
        : Primitive(o, c)
    {
        mDisplayListId = i;
        for(int j = 0; j < 3; j++) mPosition[j] = position[j];
        for(int j = 0; j < 4; j++) mRotation[j] = rotation[j];
    }

    void DisplayListPrimitive::setDisplayListId(unsigned int i) {
        mDisplayListId = i;
    }

    unsigned int DisplayListPrimitive::getDisplayListId() const {
        return mDisplayListId;
    }

    void DisplayListPrimitive::render() {
		glPushMatrix();
		glTranslatef(mPosition[0], mPosition[1], mPosition[2]);
		glRotatef(mRotation[0], mRotation[1], mRotation[2], mRotation[3]);
        glCallList(mDisplayListId);
		glPopMatrix();
    }

}


void createCSGList(unsigned int displayListID, tPart *particles, unsigned int npart, float boxSize){

    init_boxSize = boxSize;

    OpenCSG::setOption(OpenCSG::AlgorithmSetting, OpenCSG::SCS);
    OpenCSG::setOption(OpenCSG::DepthComplexitySetting, OpenCSG::OcclusionQuery);
    
    collection.resize(npart + 1);
    
    boxDL = glGenLists(1);
    glNewList(boxDL, GL_COMPILE);
    glutSolidCube(boxSize);
    glEndList();
    
    float position[] = {boxSize / 2.0f, boxSize / 2.0f, boxSize / 2.0f};
    float rotation[] = {0.0f, 1.0f, 0.0f, 0.0f};
    
    collection[0] = new OpenCSG::DisplayListPrimitive(boxDL, OpenCSG::Intersection, 1, position, rotation);
    
    for(uint i = 1; i < npart + 1; i++){
        collection[i] = new OpenCSG::DisplayListPrimitive(displayListID, OpenCSG::Subtraction, 1, particles[i].coords, particles[i].rotation);
    }
}

void clearCSGList(void){
    for(uint i = 0; i < collection.size(); i++){
        delete collection[i];
    }
    collection.clear();
    glDeleteLists(boxDL, 1);
}


void renderPrimitives(void){
    for (std::vector<OpenCSG::Primitive*>::const_iterator i = collection.begin() ; i != collection.end(); ++i) {
        (*i)->render();
    }
}

void CSGrender(void){
    OpenCSG::render(collection);
}


void CSGResizeBox(float newboxsize){

    glDeleteLists(boxDL, 1);
    delete collection[0];
    
    boxDL = glGenLists(1);
    glNewList(boxDL, GL_COMPILE);
    glutSolidCube(newboxsize);
    glEndList();
    
    float position[] = {init_boxSize / 2.0f, init_boxSize / 2.0f, init_boxSize / 2.0f};
    float rotation[] = {0.0f, 1.0f, 0.0f, 0.0f};
    
    collection[0] = new OpenCSG::DisplayListPrimitive(boxDL, OpenCSG::Intersection, 1, position, rotation);
}
