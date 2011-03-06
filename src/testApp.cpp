/***********************************************************************
 
 Demo's the usage of VertexArrays, VBOs, and PointSprites in OpenGL
 using openFrameworks 006+
 
 Mac xcode project provided, but should also work on win, linux, ios (if immediate mode code is removed)
 
 ref:
 http://www.songho.ca/opengl/gl_vertexarray.html
 http://www.songho.ca/opengl/gl_vbo.html
 
 
 If this project doesn't compile with your current versino of openframeworks, try:
 - create a new openframeworks project (duplicate an existing empty example or use a template)
 - replace the testApp.h and testApp.cpp in the new folder, with these ones.
 
 
 
 Copyright (c) 2008, Memo Akten, www.memo.tv
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 ***********************************************************************/

#include "testApp.h"

#define MAX_PARTICLES	1100000

enum DrawMode {
    kModeNormal,
    kModeVA,
    kModeVBO
};


float	pos[MAX_PARTICLES][3];
float	vel[MAX_PARTICLES][3];
float	col[MAX_PARTICLES][4];

int			numParticles		= 500000;
int			adder				= 10000;
float		currentRot			= 0;
DrawMode    drawMode            = kModeNormal;
bool		bPause				= false;
bool		useTexture			= true;
bool		useSprite			= true;
bool		vsync				= false;
bool		vboInited			= false;
int			pointSize			= 16;

GLuint		vbo[2];

ofImage		image;

//--------------------------------------------------------------
void testApp::setup(){	
	ofBackground(0,0,0);
	
	for(int i=0; i<MAX_PARTICLES; i++) {
		pos[i][0] = ofRandom(-ofGetWidth(), ofGetWidth());
		pos[i][1] = ofRandom(-ofGetHeight(), 2 * ofGetHeight());
		pos[i][2] = ofRandom(-ofGetWidth(), ofGetWidth());
		
		vel[i][0] = ofRandom(-1, 1); 
		vel[i][1] = ofRandom(-1, 1); 
		vel[i][2] = ofRandom(-1, 1); 
		
		col[i][0] = 1;//ofRandom(0, 1);
		col[i][1] = 1;//ofRandom(0, 1);
		col[i][2] = 1;//ofRandom(0, 1);
		col[i][3] = 1;//ofRandom(0, 1);
	}
	
	ofDisableArbTex();				// new in OF006, force any texture created from this point on, to be GL_TEXTURE_2D (normalized uv coords)
	image.loadImage("grad.png");
}

void testApp::exit() {
    if(vboInited) glDeleteBuffersARB(2, vbo);
}

//--------------------------------------------------------------
void testApp::update(){
	if(bPause) return;
	
	currentRot += 0.2;
	
	float r = sin(ofGetElapsedTimef()) * 0.5f + 0.5f;
	float g = cos(ofGetElapsedTimef()) * 0.5f + 0.5f;
	float b = sin(ofGetElapsedTimef()*0.5) * 0.5f + 0.5f;
	for(int i=0; i<numParticles; i++) {
		pos[i][0] += vel[i][0];
		pos[i][1] += vel[i][1];
		pos[i][2] += vel[i][2];
		
		col[i][0] = r;
		col[i][1] = g;
		col[i][2] = b;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	glPushMatrix();
	
	glTranslatef(ofGetWidth()/2, 0, 0);
	glRotatef(currentRot, 0, 1, 0);
	
	ofSetColor(255);
	
	glEnable(GL_DEPTH_TEST);
	glPointSize(pointSize);
	
	if(useTexture) image.getTextureReference().bind();		// new in OF006
	if(useSprite) {
		glEnable(GL_POINT_SPRITE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	}	
	
    switch(drawMode) {
        case kModeVA:
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            
            glVertexPointer(3, GL_FLOAT, 0, pos);
            glColorPointer(4, GL_FLOAT, 0, col);
            
            glDrawArrays(GL_POINTS, 0, numParticles);
            
            glDisableClientState(GL_VERTEX_ARRAY); 
            glDisableClientState(GL_COLOR_ARRAY);
            break;
            
        case kModeVBO:
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
            glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, numParticles * 3 * sizeof(float), pos);
            glVertexPointer(3, GL_FLOAT, 0, 0);
            
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[1]);
            glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, numParticles * 4 * sizeof(float), col);
            glColorPointer(4, GL_FLOAT, 0, 0);
            
            
            glDrawArrays(GL_POINTS, 0, numParticles);
            
            glDisableClientState(GL_VERTEX_ARRAY); 
            glDisableClientState(GL_COLOR_ARRAY);
            
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            break;
            
        case kModeNormal:
        default:
            glBegin(GL_POINTS);
            for(int i=0; i<numParticles; i++) {
                glColor4f(col[i][0], col[i][1], col[i][2], col[i][3]);
                glVertex3f(pos[i][0], pos[i][1], pos[i][2]);
            }
            glEnd();
	}
	
	if(useTexture) image.getTextureReference().unbind();		// new in OF006
	
	if(useSprite) {
		glDisable(GL_POINT_SPRITE);
	}
	
	glDisable(GL_DEPTH_TEST);	
	glPopMatrix();
    
    // black rectangle for text
	ofSetColor(0);
	ofRect(0, 0, 200, 200);
    
	ofSetColor(255);
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate(), 2) + "\n" + 
					   "numParticles: " + ofToString(numParticles) + "\n" + 
                       "immediate mode (1): " + (drawMode == kModeNormal ? "X" : "") + "\n" + 
                       "useVA (2): " + (drawMode == kModeVA ? "X" : "") + "\n" + 
                       "useVBO (3): " + (drawMode == kModeVBO ? "X" : "") + "\n" + 
                       "useTexture (t): " + (useTexture ? "X" : "") + "\n" + 
                       "useSprite (s): " + (useSprite ? "X" : "") + "\n" + 
                       "vsync (v): " + (vsync ? "X" : "") + "\n" + 
                       "pointSize []: " + ofToString(pointSize)
                       
                       , 20, 20);
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
    switch(key) {
        case '+':
            if(numParticles < MAX_PARTICLES - 1 - adder) numParticles += adder;
            break;
        case '-':
            if(numParticles > adder) numParticles -= adder;
            break;
        case '1':
            drawMode = kModeNormal;
            break;
        case '2':
            drawMode = kModeVA;
            break;
        case '3':
            drawMode = kModeVBO;
            if(!vboInited) {
                vboInited = true;
                
                // initialize VBO
                glGenBuffersARB(2, vbo);
                
                // vbo for vertex positions
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
                glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(pos), pos, GL_STREAM_DRAW_ARB);
                //	
                //	// vbo for vertex colors
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[1]);
                glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(col), col, GL_STREAM_DRAW_ARB);
                
            }
            break;
            
        case '[':
            if(pointSize>1) pointSize--;
            break;
        case ']':
            pointSize++;
            break;
        case 'v':
            vsync ^= true;
            ofSetVerticalSync(vsync);
            break;
        case 'p':
            bPause ^= true;
            break;
        case 't':
            useTexture ^=true;
            break;
        case 's':
            useSprite ^= true;
            break;
    }
    
}
