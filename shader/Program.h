/*
 * Program.h
 *
 *  Created on: 23/09/2013
 *      Author: remnanjona
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <memory>
#include <GL/gl.h>

#include "Shader.h"
#include "../buffer/VertexBuffer.h"
#include "../geometry/Geometry.h"
#include "../texture/Tex.h"


namespace std {

class Program {
private:
	/*
	 * shaders
	 */
	GLuint programID;
	Shader vert;
	Shader frag;

	/*
	 * vbo
	 */
	VertexBuffer vb;

	/*
	 * uniforms
	 */
	GLuint CameraID;
	GLuint LightID;
	GLuint ModelView3x3MatrixID;
    GLuint MatrixID;
    GLuint ViewMatrixID;
    GLuint ModelMatrixID;

	/* Textures */
	Tex *diffuseTex;
	Tex *normalTex;
	GLuint DiffuseTextureID;
	GLuint NormalTextureID;
	GLuint SpecularTextureID;

	float t;


public:
	Program();
	virtual ~Program();

	void setup( shared_ptr<Geometry> d );
	void enable();
};

} /* namespace std */
#endif /* PROGRAM_H_ */
