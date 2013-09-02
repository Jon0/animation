//---------------------------------------------------------------------------
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
// Copyright (c) 2012 by Taehyun Rhee
//
// Edited by Roma Klapaukh, Daniel Atkins, and Taehyun Rhee
//
//---------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <GL/glut.h>

#include "Skeleton.h"

static float rad_to_deg = 360.0/(2 * M_PI);

Skeleton::Skeleton( int num, bone *bones ) {
	numBones = num;
	root = bones;
	drawnState = NULL;
	select = NULL;
	idle = NULL;
	selIndex = -1;
	show_animate = false;
	animate_frame = 0.0;
	frame_rate = 0.01;

	colors[0] = 0;			// id
	colors[1] = 0xff0000ff;	// red
	colors[2] = 0xff00ff00;	// green
	colors[3] = 0xffff0000;	// blue
	colors[4] = 0xffffffff;	// white
	colors[5] = 0xffffff00;	// teal
	colors[6] = 0xff00ffff;	// yellow

	cAsId = new color();
	cStandard = new color();
	int **a = (int **)cStandard;
	int **b = (int **)cAsId;
	for (int i = 0; i < 6; ++i) {
		a[i] = &colors[i+1];
		b[i] = &colors[0];
	}

	// default colour function
	cf = &Skeleton::colorStandard;

	// set state
	addState();
}

Skeleton::~Skeleton() {
	deleteBones(root);
}

void Skeleton::deleteBones(bone* root) {
	for (int i = 0; i < numBones; i++) {
		if (root[i].name != NULL) {
			free(root[i].name);
		}
		if (root[i].children != NULL) {
			free(root[i].children);
		}
	}
	free(root);
}

void Skeleton::addState() {
	if (animation.size() > 0) {
		animation.push_back( copyState( animation.back() ) );
	}
	else {
		animation.push_back(makeState());
	}
	animate_frame = animation.size() - 1;
	drawnState = animation.back();
	drawnState_n = animation.back();
}

// [Assignment2] you may need to revise this function
void Skeleton::display() {
	if ( root == NULL || drawnState == NULL ) {
		return;
	}

	glPushMatrix();

	GLUquadric* quad = gluNewQuadric(); //Create a new quadric to allow you to draw cylinders
	if (quad == 0) {
		printf("Not enough memory to allocate space to draw\n");
		return;
	}
	//Actually draw the skeleton
	display(root, quad);

	gluDeleteQuadric(quad);
	glPopMatrix();

	if (show_animate && animation.size() > 0) {
		animate_frame = fmod(animate_frame + frame_rate, animation.size());
		drawnState = animation.at((int)animate_frame);
		drawnState_n = animation.at( ((int)animate_frame + 1) % animation.size() );
	}
}

// [Assignment2] you need to fill this function
void Skeleton::display(bone* root, GLUquadric* q) {
	if (root == NULL) {
		return;
	}
	color *cl = (this->*cf)(root);

	//gluQuadricDrawStyle(q, r);
	state_rot *c_rot = drawnState->part[root->index];
	state_rot *n_rot = drawnState_n->part[root->index];
	glPushMatrix();
	if ((root->dof & DOF_ROOT) == DOF_ROOT) {
		glTranslatef(drawnState->centre.x, drawnState->centre.y, drawnState->centre.z);
	}

	glRotatef(root->rotz, 0, 0, 1);
	glRotatef(root->roty, 0, 1, 0);
	glRotatef(root->rotx, 1, 0, 0);

	// rgb axis display
	glColor4ubv((unsigned char *) cl->x);
	display_cylinder(q, 1, 0, 0, 1, true);
	glColor4ubv((unsigned char *) cl->y);
	display_cylinder(q, 0, 1, 0, 1, true);
	glColor4ubv((unsigned char *) cl->z);
	display_cylinder(q, 0, 0, 1, 1, true);

	float time = fmod(animate_frame, 1.0);
	float a = c_rot->degree[0]*(1-time) + n_rot->degree[0]*time;
	float b = c_rot->degree[1]*(1-time) + n_rot->degree[1]*time;
	float c = c_rot->degree[2]*(1-time) + n_rot->degree[2]*time;
	glRotatef(a, 0, 0, 1);
	glRotatef(b, 0, 1, 0);
	glRotatef(c, 1, 0, 0);

	glRotatef(root->rotx, -1, 0, 0);
	glRotatef(root->roty, 0, -1, 0);
	glRotatef(root->rotz, 0, 0, -1);

	if (root == select) {
		glColor4ubv((unsigned char *) cl->select);
		gluSphere(q, 0.50, 12, 12);
	}
	else {
		glColor4ubv((unsigned char *) cl->sphere);
		gluSphere(q, 0.25, 12, 12);
		glColor4ubv((unsigned char *) cl->bone);
	}

	display_cylinder(q, root->dirx, root->diry, root->dirz, root->length, false);

	// draw children, translate into position
	glPushMatrix();
	glTranslatef(root->dirx*root->length, root->diry*root->length, root->dirz*root->length);
	for (int i = 0; i < root->numChildren; ++i) {
		display( root->children[i], q );
	}
	glPopMatrix();
	glPopMatrix();
}

void Skeleton::display_cylinder(GLUquadric* q, float x, float y, float z, float length, bool arrow) {
	Vec3D z_vec, d_vec;
	z_vec.x = 0;	z_vec.y = 0;	z_vec.z = 1;
	d_vec.x = x;	d_vec.y = y;	d_vec.z = z;
	Vec3D *a = &z_vec, *b = &d_vec;
	*a = a->crossProduct(*b);

	// dot product simplifies to z axis
	float angle = rad_to_deg * acos(z);
	float d = sqrt(a->x*a->x + a->y*a->y + a->z*a->z);

	// draw cylinder, rotated in correct direction
	glPushMatrix();

	if (d > 0) {
		glRotatef(angle, a->x, a->y, a->z);
	}
	gluCylinder(q, 0.08, 0.08, length, 10, 10);

	if (arrow) {
		glTranslatef(0.0, 0.0, length);
		glutSolidCone(0.16, 0.16, 8, 2);
	}

	glPopMatrix();
}

void Skeleton::defualtPose(bool reset) {
	// reset recreates initial pose
	if (reset) {
		if (idle) delete idle;
		idle = makeState();
	}
	else if (!idle) {
		idle = makeState();
	}
	animate(false);
	drawnState = idle;
}

void Skeleton::animate(bool a) {
	select = NULL;
	show_animate = a;
}

void Skeleton::setPlaySpeed(int s) {
	frame_rate = s;
}

int Skeleton::selectMouse(int x, int y, GLfloat *proj, GLfloat *model) {
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, y, 1, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	cf = &Skeleton::colorAsID;
	display();
	cf = &Skeleton::colorStandard;

	glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pix);
	setSelection((int)pix[0] - 1);
	glDisable(GL_SCISSOR_TEST);

	return selIndex;
}

void Skeleton::setSelection(int i) {
	if (i < 0) {
		selIndex = -1;
		select = NULL;
		return;
	}
	show_animate = false;
	selIndex = i % numBones;
	select = &root[selIndex];
}

void Skeleton::modSelection(float x, float y, float z) {
	if ( drawnState == NULL || select == NULL ) {
		return;
	}
	if ((select->dof & DOF_RX) == DOF_RX) {
		drawnState->part[select->index]->degree[0] += x;
	}
	if ((select->dof & DOF_RY) == DOF_RY) {
		drawnState->part[select->index]->degree[1] += y;
	}
	if ((select->dof & DOF_RZ) == DOF_RZ) {
		drawnState->part[select->index]->degree[2] += z;
	}
}

bool Skeleton::hasSelection() {
	return select;
}

Vec3D *Skeleton::getCentre() {
	return &drawnState->centre;
}

state *Skeleton::makeState() {
	state *current = new state();
	current->part = new state_rot *[numBones];

	for (int i = 0; i < numBones; ++i) {
		current->part[i] = new state_rot();
		current->part[i]->degree = new float [ 3 ];
		current->part[i]->degree[0] = 0;
		current->part[i]->degree[1] = 0;
		current->part[i]->degree[2] = 0;
		current->part[i]->size = 3;
	}
	return current;
}

state *Skeleton::copyState(state *other) {
	state *current = new state();
	current->part = new state_rot *[numBones];

	for (int i = 0; i < numBones; ++i) {
		current->part[i] = new state_rot();
		current->part[i]->degree = new float [ 3 ];
		current->part[i]->degree[0] = other->part[i]->degree[0];
		current->part[i]->degree[1] = other->part[i]->degree[1];
		current->part[i]->degree[2] = other->part[i]->degree[2];
		current->part[i]->size = 3;
	}
	return current;
}

void Skeleton::setFrame(int i) {
	animate_frame = i;
	drawnState = animation.at(i);
	drawnState_n = animation.at(i);
}

int Skeleton::getFrame() {
	return animate_frame;
}

color *Skeleton::colorAsID(bone *b) {
	colors[0] = b->index + 1;
	return cAsId;
}

color *Skeleton::colorStandard(bone *b) {
	return cStandard;
}


