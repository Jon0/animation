/*
 * Cube.h
 *
 *  Created on: 11/05/2013
 *      Author: remnanjona
 */

#ifndef CUBE_H_
#define CUBE_H_

#include "Geometry.h"
#include "../buffer/VertexBuffer.h"

namespace std {

class Cube:
		public Geometry
{
	float size;
	long index;

	/* Indices */
	GLuint elementbuffer;
public:
	Cube(float);
	virtual ~Cube();

	void init(VertexBuffer *);
	virtual void draw();
	virtual void drawDebug();
	virtual glm::mat4 transform();
	virtual void setTransform(glm::mat4);
};

} /* namespace std */
#endif /* CUBE_H_ */
