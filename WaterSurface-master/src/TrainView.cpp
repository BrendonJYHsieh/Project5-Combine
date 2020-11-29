/************************************************************************
     File:        TrainView.cpp

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu

     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     
						The TrainView is the window that actually shows the 
						train. Its a
						GL display canvas (Fl_Gl_Window).  It is held within 
						a TrainWindow
						that is the outer window with all the widgets. 
						The TrainView needs 
						to be aware of the window - since it might need to 
						check the widgets to see how to draw

	  Note:        we need to have pointers to this, but maybe not know 
						about it (beware circular references)

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <iostream>
#include <Fl/fl.h>

// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>
//#include "GL/gl.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include<glm/matrix.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glu.h>

#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"
using namespace std;

//My function
Pnt3f TrainView::GMT(Pnt3f p1, Pnt3f p2, Pnt3f p3, Pnt3f p4, float mode, float t) {
	glm::mat4x4 G = {
		{p1.x,p2.x,p3.x,p4.x},
		{p1.y,p2.y,p3.y,p4.y},
		{p1.z,p2.z,p3.z,p4.z},
		{1,1,1,1}
	};
	G = glm::transpose(G);
	glm::mat4x4 M;
	if (mode == 1) {
		M = { 0, 0, 0, 0,
			0, 0, -1, 1,
			0, 0, 1, 0,
			0, 0, 0, 0 };
	}
	else if (mode == 2) {
		M = {
				{-1.0f,2.0f,-1.0f,0.0f},
				{2.0f / tense - 1.0f,1.0f - 3.0f / tense,0.0f,1.0f / tense},
				{1.0f - 2.0f / tense,3.0f / tense - 2.0f,1.0f,0.0f},
				{1.0f,-1.0f,0.0f,0.0f}
		};
		M *= tense;
	}
	else if (mode == 3) {
		M = {
			{-1.0f,3.0f,-3.0f,1.0f},
			{3.0f,-6.0f,0.0f,4.0f},
			{-3.0f,3.0f,3.0f,1.0f},
			{1.0f,0.0f,0.0f,0.0f}
		};
		M /= 6.0f;
	}
	M = glm::transpose(M);
	glm::vec4 T = { pow(t,3),pow(t,2),pow(t,1),pow(t,0) };
	glm::vec4 result = G * M * T;
	return Pnt3f(result[0], result[1], result[2]);
}
void glVertex3f_Simplify(Pnt3f q0) {
	glVertex3f(q0.x, q0.y, q0.z);
}
void initDirLight() {
	float noAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
	float whiteDiffuse[] = { 1.0,1.0f ,0.0f ,1.0f };
	float position[] = { 50.0f,10.0f ,50.0f ,0.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, noAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
}
void initPosLight() {
	float yellowAmbientDiffuse[] = { 1.0f,1.0f ,1.0f ,1.0f };
	float whiteDiffuse[] = { 0.0,0.0f ,1.0f ,1.0f };
	float position[] = { 0.0f,20.0f ,0.0f ,1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, yellowAmbientDiffuse);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, whiteDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);
}
void DrawSleeper(Pnt3f qt0, Pnt3f qt1, Pnt3f cross_t, Pnt3f orient_t, bool doingShadows) {
	//礶臟瓂絬
	if (!doingShadows) {
		glColor3ub(0, 0, 0);
	}
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glVertex3f_Simplify(qt1 + cross_t);
	glVertex3f_Simplify(qt1 - cross_t);

	glVertex3f_Simplify(qt0 + cross_t - orient_t);
	glVertex3f_Simplify(qt0 - cross_t - orient_t);
	glVertex3f_Simplify(qt1 + cross_t - orient_t);
	glVertex3f_Simplify(qt1 - cross_t - orient_t);

	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 + cross_t - orient_t);
	glVertex3f_Simplify(qt1 + cross_t);
	glVertex3f_Simplify(qt1 + cross_t - orient_t);

	glVertex3f_Simplify(qt0 - cross_t);
	glVertex3f_Simplify(qt0 - cross_t - orient_t);
	glVertex3f_Simplify(qt1 - cross_t);
	glVertex3f_Simplify(qt1 - cross_t - orient_t);


	glVertex3f_Simplify(qt0 - cross_t - orient_t);
	glVertex3f_Simplify(qt1 - cross_t - orient_t);


	glVertex3f_Simplify(qt0 + cross_t - orient_t);
	glVertex3f_Simplify(qt1 + cross_t - orient_t);
	glEnd();

	//礶臟瓂
	if (!doingShadows) {
		glColor3ub(101, 50, 0);
	}
	glNormal3f(orient_t.x, orient_t.y, orient_t.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt1 + cross_t);
	glVertex3f_Simplify(qt1 - cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glEnd();

	glNormal3f(-orient_t.x, -orient_t.y, -orient_t.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 - orient_t + cross_t);
	glVertex3f_Simplify(qt1 - orient_t + cross_t);
	glVertex3f_Simplify(qt1 - orient_t - cross_t);
	glVertex3f_Simplify(qt0 - orient_t - cross_t);
	glEnd();

	glNormal3f(-(qt1 - qt0).x, -(qt1 - qt0).y, -(qt1 - qt0).z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glVertex3f_Simplify(qt0 - orient_t - cross_t);
	glVertex3f_Simplify(qt0 - orient_t + cross_t);
	glEnd();

	glNormal3f((qt1 - qt0).x, (qt1 - qt0).y, (qt1 - qt0).z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt1 + cross_t);
	glVertex3f_Simplify(qt1 - cross_t);
	glVertex3f_Simplify(qt1 - orient_t - cross_t);
	glVertex3f_Simplify(qt1 - orient_t + cross_t);
	glEnd();

	glNormal3f(-cross_t.x, -cross_t.y, -cross_t.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt1 - cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glVertex3f_Simplify(qt0 - orient_t - cross_t);
	glVertex3f_Simplify(qt1 - orient_t - cross_t);
	glEnd();

	glNormal3f(cross_t.x, cross_t.y, cross_t.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt1 + cross_t);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 - orient_t + cross_t);
	glVertex3f_Simplify(qt1 - orient_t + cross_t);
	glEnd();
}
void DrawPillar(Pnt3f qt0, Pnt3f qt1, Pnt3f cross_t, Pnt3f orient_t, bool doingShadows) {
	/*琖*/
	if (!doingShadows) {
		glColor3ub(64, 17, 140);
	}
	//
	glBegin(GL_QUADS);
	glVertex3f((qt0 + cross_t).x, 0.1, (qt0 + cross_t).z);
	glVertex3f((qt1 + cross_t).x, 0.1, (qt1 + cross_t).z);
	glVertex3f((qt1 - cross_t).x, 0.1, (qt1 - cross_t).z);
	glVertex3f((qt0 - cross_t).x, 0.1, (qt0 - cross_t).z);
	glEnd();
	//
	glNormal3f(cross_t.x, cross_t.y, cross_t.z);
	glBegin(GL_QUADS);
	glVertex3f((qt0 + cross_t).x, (qt0 + cross_t).y, (qt0 + cross_t).z);
	glVertex3f((qt1 + cross_t).x, (qt1 + cross_t).y, (qt1 + cross_t).z);
	glVertex3f((qt1 + cross_t).x, 0.1, (qt1 + cross_t).z);
	glVertex3f((qt0 + cross_t).x, 0.1, (qt0 + cross_t).z);
	glEnd();
	//オ
	glNormal3f(-cross_t.x, -cross_t.y, -cross_t.z);
	glBegin(GL_QUADS);
	glVertex3f((qt0 - cross_t).x, (qt0 - cross_t).y, (qt0 - cross_t).z);
	glVertex3f((qt1 - cross_t).x, (qt1 - cross_t).y, (qt1 - cross_t).z);
	glVertex3f((qt1 - cross_t).x, 0.1, (qt1 - cross_t).z);
	glVertex3f((qt0 - cross_t).x, 0.1, (qt0 - cross_t).z);
	glEnd();
	//玡
	glNormal3f((qt1 - qt0).x, (qt1 - qt0).y, (qt1 - qt0).z);
	glBegin(GL_QUADS);
	glVertex3f((qt1 - cross_t).x, (qt1 - cross_t).y, (qt1 - cross_t).z);
	glVertex3f((qt1 + cross_t).x, (qt1 - cross_t).y, (qt1 + cross_t).z);
	glVertex3f((qt1 + cross_t).x, 0.1, (qt1 + cross_t).z);
	glVertex3f((qt1 - cross_t).x, 0.1, (qt1 - cross_t).z);
	glEnd();
	//
	glNormal3f(-(qt1 - qt0).x, -(qt1 - qt0).y, -(qt1 - qt0).z);
	glBegin(GL_QUADS);
	glVertex3f((qt0 - cross_t).x, (qt0 - cross_t).y, (qt0 - cross_t).z);
	glVertex3f((qt0 + cross_t).x, (qt0 - cross_t).y, (qt0 + cross_t).z);
	glVertex3f((qt0 + cross_t).x, 0.1, (qt0 + cross_t).z);
	glVertex3f((qt0 - cross_t).x, 0.1, (qt0 - cross_t).z);
	glEnd();
	/*琖*/
}

void DrawTrain(Pnt3f qt0, Pnt3f cross_t, Pnt3f up, Pnt3f forward, bool doingShadows) {
	if (!doingShadows) {
		glColor3ub(100, 0, 0);
	}
	glNormal3f(up.x, up.y, up.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 + cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t + up);
	glVertex3f_Simplify(qt0 + forward - cross_t + up);
	glVertex3f_Simplify(qt0 - cross_t + up);
	glEnd();
	//
	glNormal3f(-up.x, -up.y, -up.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 + forward + cross_t);
	glVertex3f_Simplify(qt0 + forward - cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glEnd();
	//玡
	glNormal3f(forward.x, forward.y, forward.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 + forward - cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t);
	glVertex3f_Simplify(qt0 + forward - cross_t);
	glEnd();
	//
	glNormal3f(-forward.x, -forward.y, -forward.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 - cross_t + up);
	glVertex3f_Simplify(qt0 + cross_t + up);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glEnd();
	//
	glNormal3f(cross_t.x, cross_t.y, cross_t.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 + cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t);
	
	
	glEnd();
	//オ
	glNormal3f(-cross_t.x, -cross_t.y, -cross_t.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 + forward - cross_t + up);
	glVertex3f_Simplify(qt0 + forward - cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glVertex3f_Simplify(qt0 - cross_t + up);
	glEnd();
	
	if (!doingShadows) {
		glColor3ub(0, 0, 0);
	}
	//近
	glBegin(GL_POLYGON);
	glVertex3f_Simplify(qt0 - cross_t * 1.2);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 0.1);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 0.2 + up * 0.2);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 0.1 + up * 0.4);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + up * 0.4);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 - forward * 0.1 + up * 0.2);
	glEnd();
	glFlush();
	//玡近
	glBegin(GL_POLYGON);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 0.9);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 1);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 1.1 + up * 0.2);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 1 + up * 0.4);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + up * 0.4 + forward * 0.9);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 0.8 + up * 0.2);
	glEnd();
	glFlush();
	//玡近
	glBegin(GL_POLYGON);
	glVertex3f_Simplify(qt0 + cross_t * 1.2);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 0.1);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 0.2 + up * 0.2);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 0.1 + up * 0.4);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + up * 0.4);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 - forward * 0.1 + up * 0.2);
	glEnd();
	glFlush();
	//玡近                  
	glBegin(GL_POLYGON);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 0.9);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 1);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 1.1 + up * 0.2);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 1 + up * 0.4);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + up * 0.4 + forward * 0.9);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 0.8 + up * 0.2);
	glEnd();
}

void DrawTrainHead(Pnt3f qt0, Pnt3f cross_t, Pnt3f up, Pnt3f forward, bool doingShadows) {
	if (!doingShadows) {
		glColor3ub(0, 30, 150);
	}
	forward = forward * 1.5;
	//
	glBegin(GL_QUADS);
	glNormal3f(up.x, up.y, up.z);
	glVertex3f_Simplify(qt0 + cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t + up);
	glVertex3f_Simplify(qt0 + forward - cross_t + up);
	glVertex3f_Simplify(qt0 - cross_t + up);
	glEnd();
	//
	glBegin(GL_QUADS);
	glNormal3f(-up.x, -up.y, -up.z);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 + forward + cross_t);
	glVertex3f_Simplify(qt0 + forward - cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glEnd();
	//玡
	glBegin(GL_QUADS);
	glNormal3f(forward.x, forward.y, forward.z);
	glVertex3f_Simplify(qt0 + forward - cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t);
	glVertex3f_Simplify(qt0 + forward - cross_t);
	glEnd();
	//
	glBegin(GL_QUADS);
	glNormal3f(-forward.x, -forward.y, -forward.z);
	glVertex3f_Simplify(qt0 - cross_t + up);
	glVertex3f_Simplify(qt0 + cross_t + up);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glEnd();
	//オ
	glNormal3f(-cross_t.x, -cross_t.y, -cross_t.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 + forward - cross_t + up);
	glVertex3f_Simplify(qt0 + forward - cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glVertex3f_Simplify(qt0 - cross_t + up);
	glEnd();
	//
	glNormal3f(cross_t.x, cross_t.y, cross_t.z);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 + forward + cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 + cross_t + up);
	glEnd();
	/*场だ*/
	float height = 1.8f;
	float length = 0.4f;
	cross_t = cross_t * 1.2;
	//
	glBegin(GL_QUADS);
	glNormal3f(up.x, up.y, up.z);
	glVertex3f_Simplify(qt0 + cross_t + up * height);
	glVertex3f_Simplify(qt0 + forward * length + cross_t + up * height);
	glVertex3f_Simplify(qt0 + forward * length - cross_t + up * height);
	glVertex3f_Simplify(qt0 - cross_t + up * height);
	glEnd();
	//
	glBegin(GL_QUADS);
	glNormal3f(-forward.x, -forward.y, -forward.z);
	glVertex3f_Simplify(qt0 - cross_t + up * height);
	glVertex3f_Simplify(qt0 + cross_t + up * height);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glEnd();
	//玡
	glBegin(GL_QUADS);
	glNormal3f(forward.x, forward.y, forward.z);
	glVertex3f_Simplify(qt0 + forward * length - cross_t + up * height);
	glVertex3f_Simplify(qt0 + forward * length + cross_t + up * height);
	glVertex3f_Simplify(qt0 + forward * length + cross_t);
	glVertex3f_Simplify(qt0 + forward * length - cross_t);
	glEnd();
	//オ
	glBegin(GL_QUADS);
	glNormal3f(-cross_t.x, -cross_t.y, -cross_t.z);
	glVertex3f_Simplify(qt0 + forward * length - cross_t + up * height);
	glVertex3f_Simplify(qt0 + forward * length - cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glVertex3f_Simplify(qt0 - cross_t + up * height);
	glEnd();
	//
	glBegin(GL_QUADS);
	glNormal3f(cross_t.x, cross_t.y, cross_t.z);
	glVertex3f_Simplify(qt0 + forward * length + cross_t + up * height);
	glVertex3f_Simplify(qt0 + forward * length + cross_t);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 + cross_t + up * height);
	glEnd();
	cross_t = cross_t / 1.2;
	forward = forward / 1.5;
	if (!doingShadows) {
		glColor3ub(0, 0, 0);
	}
	//近
	glBegin(GL_POLYGON);
	glVertex3f_Simplify(qt0 - cross_t * 1.4);
	glVertex3f_Simplify(qt0 - cross_t * 1.4 + forward * 0.1);
	glVertex3f_Simplify(qt0 - cross_t * 1.4 + forward * 0.2 + up * 0.2);
	glVertex3f_Simplify(qt0 - cross_t * 1.4 + forward * 0.1 + up * 0.4);
	glVertex3f_Simplify(qt0 - cross_t * 1.4 + up * 0.4);
	glVertex3f_Simplify(qt0 - cross_t * 1.4 - forward * 0.1 + up * 0.2);
	glEnd();
	glFlush();
	//玡近
	glBegin(GL_POLYGON);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 0.9);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 1);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 1.1 + up * 0.2);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 1 + up * 0.4);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + up * 0.4 + forward * 0.9);
	glVertex3f_Simplify(qt0 - cross_t * 1.2 + forward * 0.8 + up * 0.2);
	glEnd();
	glFlush();
	//玡近
	glBegin(GL_POLYGON);
	glVertex3f_Simplify(qt0 + cross_t * 1.4);
	glVertex3f_Simplify(qt0 + cross_t * 1.4 + forward * 0.1);
	glVertex3f_Simplify(qt0 + cross_t * 1.4 + forward * 0.2 + up * 0.2);
	glVertex3f_Simplify(qt0 + cross_t * 1.4 + forward * 0.1 + up * 0.4);
	glVertex3f_Simplify(qt0 + cross_t * 1.4 + up * 0.4);
	glVertex3f_Simplify(qt0 + cross_t * 1.4 - forward * 0.1 + up * 0.2);
	glEnd();
	glFlush();
	//玡近                  
	glBegin(GL_POLYGON);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 0.9);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 1);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 1.1 + up * 0.2);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 1 + up * 0.4);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + up * 0.4 + forward * 0.9);
	glVertex3f_Simplify(qt0 + cross_t * 1.2 + forward * 0.8 + up * 0.2);
	glEnd();
}
//************************************************************************
//
// * Constructor to set up the GL window
//========================================================================
TrainView::
TrainView(int x, int y, int w, int h, const char* l) 
	: Fl_Gl_Window(x,y,w,h,l)
//========================================================================
{
	mode( FL_RGB|FL_ALPHA|FL_DOUBLE | FL_STENCIL );

	resetArcball();
}

//************************************************************************
//
// * Reset the camera to look at the world
//========================================================================
void TrainView::
resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

//************************************************************************
//
// * FlTk Event handler for the window
//########################################################################
// TODO: 
//       if you want to make the train respond to other events 
//       (like key presses), you might want to hack this.
//########################################################################
//========================================================================
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, 
	// then we're done
	// note: the arcball only gets the event if we're in world view
	if (tw->worldCam->value())
		if (arcball.handle(event)) 
			return 1;

	// remember what button was used
	static int last_push;

	switch(event) {
		// Mouse button being pushed event
		case FL_PUSH:
			last_push = Fl::event_button();
			// if the left button be pushed is left mouse button
			if (last_push == FL_LEFT_MOUSE  ) {
				doPick();
				damage(1);
				return 1;
			};
			break;

	   // Mouse button release event
		case FL_RELEASE: // button release
			damage(1);
			last_push = 0;
			return 1;

		// Mouse button drag event
		case FL_DRAG:

			// Compute the new control point position
			if ((last_push == FL_LEFT_MOUSE) && (selectedCube >= 0)) {
				ControlPoint* cp = &m_pTrack->points[selectedCube];

				double r1x, r1y, r1z, r2x, r2y, r2z;
				getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

				double rx, ry, rz;
				mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z, 
								static_cast<double>(cp->pos.x), 
								static_cast<double>(cp->pos.y),
								static_cast<double>(cp->pos.z),
								rx, ry, rz,
								(Fl::event_state() & FL_CTRL) != 0);

				cp->pos.x = (float) rx;
				cp->pos.y = (float) ry;
				cp->pos.z = (float) rz;
				damage(1);
			}
			break;

		// in order to get keyboard events, we need to accept focus
		case FL_FOCUS:
			return 1;

		// every time the mouse enters this window, aggressively take focus
		case FL_ENTER:	
			focus(this);
			break;

		case FL_KEYBOARD:
		 		int k = Fl::event_key();
				int ks = Fl::event_state();
				if (k == 'p') {
					// Print out the selected control point information
					if (selectedCube >= 0) 
						printf("Selected(%d) (%g %g %g) (%g %g %g)\n",
								 selectedCube,
								 m_pTrack->points[selectedCube].pos.x,
								 m_pTrack->points[selectedCube].pos.y,
								 m_pTrack->points[selectedCube].pos.z,
								 m_pTrack->points[selectedCube].orient.x,
								 m_pTrack->points[selectedCube].orient.y,
								 m_pTrack->points[selectedCube].orient.z);
					else
						printf("Nothing Selected\n");

					return 1;
				};
				break;
	}

	return Fl_Gl_Window::handle(event);
}

//************************************************************************
//
// * this is the code that actually draws the window
//   it puts a lot of the work into other routines to simplify things
//========================================================================
void TrainView::draw()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//initialized glad
	if (gladLoadGL())
	{
		//initiailize VAO, VBO, Shader...

		if (!this->shader)
			this->shader = new
			Shader(
				"../Project4-WaterSurface/WaterSurface-master/src/shaders/simple.vert", 
				nullptr, nullptr, nullptr, 
				"../Project4-WaterSurface/WaterSurface-master/src/shaders/simple.frag");

		if (!this->commom_matrices)
			this->commom_matrices = new UBO();
			this->commom_matrices->size = 2 * sizeof(glm::mat4);
			glGenBuffers(1, &this->commom_matrices->ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
			glBufferData(GL_UNIFORM_BUFFER, this->commom_matrices->size, NULL, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

		if (!this->plane) {
			GLfloat  vertices[] = {
				-0.5f ,0.0f , -0.5f,
				-0.5f ,0.0f , 0.5f ,
				0.5f ,0.0f ,0.5f ,
				0.5f ,0.0f ,-0.5f };
			GLfloat  normal[] = {
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f };
			GLfloat  texture_coordinate[] = {
				0.0f, 0.0f,
				1.0f, 0.0f,
				1.0f, 1.0f,
				0.0f, 1.0f };
			GLuint element[] = {
				0, 1, 2,
				0, 2, 3, };

			this->plane = new VAO;
			this->plane->element_amount = sizeof(element) / sizeof(GLuint);
			glGenVertexArrays(1, &this->plane->vao);
			glGenBuffers(3, this->plane->vbo);
			glGenBuffers(1, &this->plane->ebo);

			glBindVertexArray(this->plane->vao);

			// Position attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// Normal attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);

			// Texture Coordinate attribute
			glBindBuffer(GL_ARRAY_BUFFER, this->plane->vbo[2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coordinate), texture_coordinate, GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(2);

			//Element attribute
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->plane->ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW);

			// Unbind VAO
			glBindVertexArray(0);
		}

		if (!this->texture)
			this->texture = new Texture2D("../Project4-WaterSurface/WaterSurface-master/Images/church.png");

		if (!this->device){
			//Tutorial: https://ffainelli.github.io/openal-example/
			this->device = alcOpenDevice(NULL);
			if (!this->device)
				puts("ERROR::NO_AUDIO_DEVICE");

			ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
			if (enumeration == AL_FALSE)
				puts("Enumeration not supported");
			else
				puts("Enumeration supported");

			this->context = alcCreateContext(this->device, NULL);
			if (!alcMakeContextCurrent(context))
				puts("Failed to make context current");

			this->source_pos = glm::vec3(0.0f, 5.0f, 0.0f);

			ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
			alListener3f(AL_POSITION, source_pos.x, source_pos.y, source_pos.z);
			alListener3f(AL_VELOCITY, 0, 0, 0);
			alListenerfv(AL_ORIENTATION, listenerOri);

			alGenSources((ALuint)1, &this->source);
			alSourcef(this->source, AL_PITCH, 1);
			alSourcef(this->source, AL_GAIN, 1.0f);
			alSource3f(this->source, AL_POSITION, source_pos.x, source_pos.y, source_pos.z);
			alSource3f(this->source, AL_VELOCITY, 0, 0, 0);
			alSourcei(this->source, AL_LOOPING, AL_TRUE);

			alGenBuffers((ALuint)1, &this->buffer);

			ALsizei size, freq;
			ALenum format;
			ALvoid* data;
			ALboolean loop = AL_TRUE;

			//Material from: ThinMatrix
			alutLoadWAVFile((ALbyte*)"../WaterSurface/Audios/bounce.wav", &format, &data, &size, &freq, &loop);
			alBufferData(this->buffer, format, data, size, freq);
			alSourcei(this->source, AL_BUFFER, this->buffer);

			if (format == AL_FORMAT_STEREO16 || format == AL_FORMAT_STEREO8)
				puts("TYPE::STEREO");
			else if (format == AL_FORMAT_MONO16 || format == AL_FORMAT_MONO8)
				puts("TYPE::MONO");

			alSourcePlay(this->source);

			// cleanup context
			//alDeleteSources(1, &source);
			//alDeleteBuffers(1, &buffer);
			//device = alcGetContextsDevice(context);
			//alcMakeContextCurrent(NULL);
			//alcDestroyContext(context);
			//alcCloseDevice(device);
		}
	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	// Set up the view port
	glViewport(0,0,w(),h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,.3f,0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (tw->topCam->value()) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]			= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};
	//initDirLight();
	//initPosLight();


	/*glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);*/

	// set linstener position 
	if(selectedCube >= 0)
		alListener3f(AL_POSITION, 
			m_pTrack->points[selectedCube].pos.x,
			m_pTrack->points[selectedCube].pos.y,
			m_pTrack->points[selectedCube].pos.z);
	else
		alListener3f(AL_POSITION, 
			this->source_pos.x, 
			this->source_pos.y,
			this->source_pos.z);


	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)
	glUseProgram(0);

	setupFloor();
	glDisable(GL_LIGHTING);
	drawFloor(200,10);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (!tw->topCam->value()) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}

	setUBO();
	glBindBufferRange(
		GL_UNIFORM_BUFFER, /*binding point*/0, this->commom_matrices->ubo, 0, this->commom_matrices->size);

	//bind shader
	this->shader->Use();

	glm::mat4 model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, this->source_pos);
	model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(glGetUniformLocation(this->shader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);
	glUniform3fv(glGetUniformLocation(this->shader->Program, "u_color"), 1, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);
	this->texture->bind(0);

	glUniform1i(glGetUniformLocation(this->shader->Program, "u_texture"), 0);
	
	Pnt3f qt0(0, 0, 0);
	Pnt3f up(0, 1, 0);
	Pnt3f cross_t(1,0, 0);
	Pnt3f forward(0, 0, 1);
	
	//glColor3ub(255, 255, 255);
	/*glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);
	glVertex3f(0, 1, -1);
	glVertex3f(0, 0, -1);
	glEnd();*/
	

	//Direction light
	glUniform3f(glGetUniformLocation(this->shader->Program, "viewPos"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shader->Program, "dirLight.direction"), 0.0f, 1.5f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shader->Program, "dirLight.ambient"), 1.0f, 1.0f, 0.00f);
	glUniform3f(glGetUniformLocation(this->shader->Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
	glUniform3f(glGetUniformLocation(this->shader->Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
	//Point light
	glUniform3f(glGetUniformLocation(this->shader->Program, "pointLights.direction"), 1.0f, 0.5f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shader->Program, "pointLights.ambient"), 1.0f, 0.0f, 0.00f);
	glUniform3f(glGetUniformLocation(this->shader->Program, "pointLights.diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(this->shader->Program, "pointLights.specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(this->shader->Program, "pointLights.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shader->Program, "pointLights.linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shader->Program, "pointLights.quadratic"), 0.032f);
	//spot light
	
	glUniform3f(glGetUniformLocation(this->shader->Program, "spotLight.position"), 0.0f, 5.0f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shader->Program, "spotLight.direction"), 0.0f, -1.0f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shader->Program, "spotLight.ambient"), 1.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shader->Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(this->shader->Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(this->shader->Program, "spotLight.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shader->Program, "spotLight.linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shader->Program, "spotLight.quadratic"), 0.032f);
	glUniform1f(glGetUniformLocation(this->shader->Program, "spotLight.cutOff"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shader->Program, "spotLight.quadratic"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(this->shader->Program, "spotLight.outerCutOff"), glm::cos(glm::radians(12.5f)));


	glUniform1f(glGetUniformLocation(this->shader->Program, "material.diffuse"), 0.0f);
	glUniform1f(glGetUniformLocation(this->shader->Program, "material.specular"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shader->Program, "material.shininess"), 32.0f);
	//bind VAO
	glBindVertexArray(this->plane->vao);

	glDrawElements(GL_TRIANGLES, this->plane->element_amount, GL_UNSIGNED_INT, 0);

	//unbind VAO
	glBindVertexArray(0);

	//unbind shader(switch to fixed pipeline)
	glUseProgram(0);
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	// Check whether we use the world camp
	if (tw->worldCam->value()) {
		arcball.setProjection(false);
		//GLdouble temp[16];
		//glGetDoublev(GL_MODELVIEW_MATRIX, temp);
		///*for (int i = 0; i < 16; i++) {
		//	cout << temp[i] << " " << endl;
		//}*/
		//glm::mat4x4 modelView;
		//for (int i = 0; i < 16; i++) {
		//	modelView[i / 4][i % 4] = temp[i];
		//}
		//modelView = glm::transpose(modelView);
		////for (int i = 0; i < 4; i++) {
		////	for (int j = 0; j < 4; j++) {
		////		cout << modelView[i][j] << endl;
		////	}
		////	cout << endl;
		////}
		//modelView = glm::inverse(modelView);
	}
		

	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
	} 
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
		trainCamView(this, aspect);
	}
}
void TrainView::trainCamView(TrainView* TrainV, float aspect) {
	float percent = 1.0f / DIVIDE_LINE;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120, aspect, 0.01, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float t;
	int i;
	if (TrainV->tw->arcLength->value()) {
		t = t_t;
		i = t_i;
	}
	else {
		i = floor(t_time);
		t = t_time - i;
	}
	ControlPoint p1 = m_pTrack->points[(i - 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
	ControlPoint p2 = m_pTrack->points[(i + m_pTrack->points.size()) % m_pTrack->points.size()];
	ControlPoint p3 = m_pTrack->points[(i + 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
	ControlPoint p4 = m_pTrack->points[(i + 2 + m_pTrack->points.size()) % m_pTrack->points.size()];

	Pnt3f eye_orient = GMT(p1.orient, p2.orient, p3.orient, p4.orient, tw->splineBrowser->value(), t - percent);
	Pnt3f centery_ori = GMT(p1.orient, p2.orient, p3.orient, p4.orient, tw->splineBrowser->value(), t);

	Pnt3f eye = GMT(p1.pos, p2.pos, p3.pos, p4.pos, tw->splineBrowser->value(), t - percent) + eye_orient * 2.0f;
	Pnt3f centery = GMT(p1.pos, p2.pos, p3.pos, p4.pos, tw->splineBrowser->value(), t) + centery_ori * 2.0f;
	gluLookAt(eye.x, eye.y, eye.z, centery.x, centery.y, centery.z, eye_orient.x, eye_orient.y, eye_orient.z);
}
//****
//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (!tw->trainCam->value()) {
		for(size_t i=0; i<m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if ( ((int) i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			m_pTrack->points[i].draw();
		}
	}
	// draw the track
	drawTrack(this, doingShadows);
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
}
void TrainView::drawTrack(TrainView* TrainV, bool doingShadows) {
	Path_Total = 0;
	float Sleep_Total = 0.0f;
	float percent = 1.0f / DIVIDE_LINE;
	Pnt3f lastqt;
	bool Draw_Sleeper = false;
	bool check = false;
	int count = 0;
	//glutSolidSphere(0.7f, 15, 15);
	for (size_t i = 0; i < m_pTrack->points.size(); i++) {
		float t = 0;
		ControlPoint p1 = m_pTrack->points[(i - 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
		ControlPoint p2 = m_pTrack->points[(i + m_pTrack->points.size()) % m_pTrack->points.size()];
		ControlPoint p3 = m_pTrack->points[(i + 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
		ControlPoint p4 = m_pTrack->points[(i + 2 + m_pTrack->points.size()) % m_pTrack->points.size()];
		for (size_t j = 0; j < DIVIDE_LINE; j++) {
			Pnt3f qt0 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, TrainV->tw->splineBrowser->value(), t);
			Pnt3f orient_t = GMT(p1.orient, p2.orient, p3.orient, p4.orient, TrainV->tw->splineBrowser->value(), t);
			Pnt3f qt1 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, TrainV->tw->splineBrowser->value(), t += percent);
			Pnt3f forward = qt1 - qt0;
			Pnt3f cross_t = forward * orient_t;
			cross_t.normalize();
			orient_t = cross_t * forward;
			orient_t.normalize();
			cross_t = cross_t * Sleeper_Width;
			/*ó簿笆*/
			Path_Total += sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
			if (!check && Path_Total > TrainV->m_pTrack->trainU) {
				if (qt1.y - qt0.y > 0) {
					physical = qt0.y - qt1.y;
				}
				else if (qt1.y - qt0.y < 0) {
					physical = qt0.y - qt1.y;
					physical *= 0.7;
				}
				else {
					physical = 0;
				}
				t_t = t;
				t_i = i;
				check = true;
			}

			/*臟瓂*/
			if (!doingShadows) {
				glColor3ub(77, 19, 0);
			}
			glNormal3f(orient_t.x, orient_t.y, orient_t.z);
			glLineWidth(5);
			glBegin(GL_LINES);
			glVertex3f_Simplify(qt0 + cross_t);
			glVertex3f_Simplify(qt1 + cross_t);
			glVertex3f_Simplify(qt0 - cross_t);
			glVertex3f_Simplify(qt1 - cross_t);
			glEnd();
			//干礶臟瓂耞吊矪
			if (j != 0) {
				glNormal3f(orient_t.x, orient_t.y, orient_t.z);
				glLineWidth(5);
				glBegin(GL_LINES);
				glVertex3f_Simplify(lastqt + cross_t);
				glVertex3f_Simplify(qt1 + cross_t);
				glVertex3f_Simplify(lastqt - cross_t);
				glVertex3f_Simplify(qt1 - cross_t);
				glEnd();
			}
			Sleep_Total += sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
			if (!Draw_Sleeper && Sleep_Total >= Sleeper_Length) {
				count++;
				forward.normalize();
				DrawSleeper(qt0, qt0 - forward * Sleeper_Length, cross_t, orient_t, doingShadows);
				if (count % 3 == 0) {
					DrawPillar(qt0, qt0 - forward * Sleeper_Length, cross_t, orient_t, doingShadows);
					count = 0;
				}
				Sleep_Total -= Sleeper_Length;
				Draw_Sleeper = !Draw_Sleeper;
			}
			else if (Draw_Sleeper && Sleep_Total >= Sleeper_Interval) {
				Sleep_Total -= Sleeper_Interval;
				Draw_Sleeper = !Draw_Sleeper;
			}
			lastqt = qt0;
		}
	}
	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif
}
void TrainView::drawTrain(TrainView* TrainV, bool doingShadows) {

	float percent = 1.0f / DIVIDE_LINE;
	float t;
	int i;
	if (TrainV->tw->arcLength->value()) {
		t = t_t;
		i = t_i;
	}
	else {
		i = floor(t_time);
		t = t_time - i;
	}

	for (int j = 1; j <= car_quantity; j++) {
		if (TrainV->tw->arcLength->value()) {
			bool check = false;
			ControlPoint p1 = m_pTrack->points[(i - 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
			ControlPoint p2 = m_pTrack->points[(i + m_pTrack->points.size()) % m_pTrack->points.size()];
			ControlPoint p3 = m_pTrack->points[(i + 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
			ControlPoint p4 = m_pTrack->points[(i + 2 + m_pTrack->points.size()) % m_pTrack->points.size()];

			Pnt3f qt0 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, TrainV->tw->splineBrowser->value(), t);
			Pnt3f orient_t = GMT(p1.orient, p2.orient, p3.orient, p4.orient, TrainV->tw->splineBrowser->value(), t);
			Pnt3f qt1 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, TrainV->tw->splineBrowser->value(), t += percent);

			Pnt3f cross_t = (qt1 - qt0) * orient_t;
			cross_t.normalize();
			orient_t = cross_t * (qt1 - qt0);
			orient_t.normalize();
			cross_t = cross_t * Train_Width;
			Pnt3f up = orient_t * Train_Height;
			Pnt3f forward = (qt1 - qt0);
			forward.normalize();
			forward = forward * Train_Forward;

			float total = 0.0f;
			if (j == 1) {
				DrawTrainHead(qt0, cross_t, up, forward, doingShadows);
			}
			else {
				DrawTrain(qt0, cross_t, up, forward, doingShadows);
			}

			while (!check) {
				for (t; t >= 0; t = t - percent) {
					qt0 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, TrainV->tw->splineBrowser->value(), t);
					qt1 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, TrainV->tw->splineBrowser->value(), t + percent);
					forward = qt1 - qt0;
					total += sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
					if (total >= 13) {
						check = true;
						break;
					}
				}
				if (check) {
					break;
				}
				t = 1.0f;
				i -= 1;
				if (i < 0) {
					i = m_pTrack->points.size() - 1;
				}
				p1 = m_pTrack->points[(i - 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
				p2 = m_pTrack->points[(i + m_pTrack->points.size()) % m_pTrack->points.size()];
				p3 = m_pTrack->points[(i + 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
				p4 = m_pTrack->points[(i + 2 + m_pTrack->points.size()) % m_pTrack->points.size()];
			}
		}
		else {
			float tt = t_time - percent * 200 * (j - 1);
			int ii = floor(tt);
			tt -= ii;
			ControlPoint p1 = m_pTrack->points[(ii - 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
			ControlPoint p2 = m_pTrack->points[(ii + m_pTrack->points.size()) % m_pTrack->points.size()];
			ControlPoint p3 = m_pTrack->points[(ii + 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
			ControlPoint p4 = m_pTrack->points[(ii + 2 + m_pTrack->points.size()) % m_pTrack->points.size()];

			Pnt3f qt0 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, TrainV->tw->splineBrowser->value(), tt);
			Pnt3f orient_t = GMT(p1.orient, p2.orient, p3.orient, p4.orient, TrainV->tw->splineBrowser->value(), tt);
			Pnt3f qt1 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, TrainV->tw->splineBrowser->value(), tt += percent);

			Pnt3f cross_t = (qt1 - qt0) * orient_t;
			cross_t.normalize();
			orient_t = cross_t * (qt1 - qt0);
			orient_t.normalize();
			cross_t = cross_t * Train_Width;
			Pnt3f up = orient_t * Train_Height;
			Pnt3f forward = (qt1 - qt0);
			forward.normalize();
			forward = forward * Train_Forward;
			if (j == 1) {
				DrawTrainHead(qt0, cross_t, up, forward, doingShadows);
			}
			else {
				DrawTrain(qt0, cross_t, up, forward, doingShadows);
			}
		}
	}
}

// 
//************************************************************************
//
// * this tries to see which control point is under the mouse
//	  (for when the mouse is clicked)
//		it uses OpenGL picking - which is always a trick
//########################################################################
// TODO: 
//		if you want to pick things other than control points, or you
//		changed how control points are drawn, you might need to change this
//########################################################################
//========================================================================
void TrainView::
doPick()
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	make_current();		

	// where is the mouse?
	int mx = Fl::event_x(); 
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
						5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n",selectedCube);
}

void TrainView::setUBO()
{
	float wdt = this->pixel_w();
	float hgt = this->pixel_h();

	glm::mat4 view_matrix;
	glGetFloatv(GL_MODELVIEW_MATRIX, &view_matrix[0][0]);
	//HMatrix view_matrix; 
	//this->arcball.getMatrix(view_matrix);

	glm::mat4 projection_matrix;
	glGetFloatv(GL_PROJECTION_MATRIX, &projection_matrix[0][0]);
	//projection_matrix = glm::perspective(glm::radians(this->arcball.getFoV()), (GLfloat)wdt / (GLfloat)hgt, 0.01f, 1000.0f);

	glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection_matrix[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view_matrix[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}