/*
 * Light.cpp
 *
 *  Created on: 25/09/2013
 *      Author: remnanjona
 */

#include <iostream>
#include <GL/glu.h>

#include "LightingModel.h"

namespace std {

LightingModel::LightingModel(Program &shadow, Program &main):
		shadowMapUniform { [](GLuint i, GLuint v){ glUniform1i(i, v); } },
		modelMatrix { [](GLuint i, glm::mat4 v){ glUniformMatrix4fv(i, 1, GL_FALSE, &v[0][0]); } },
		DepthBias { [](GLuint i, glm::mat4 v){ glUniformMatrix4fv(i, 1, GL_FALSE, &v[0][0]); } },
		Positions { [](GLuint i, vector<glm::vec4> v){ glUniform4fv(i, v.size(), &v.data()[0][0]); } }
{


	Positions.data.push_back( glm::vec4(7.5, 2.0, 7.5, 1.0) );
	Positions.data.push_back( glm::vec4(-7.5, 2.0, -7.5, 1.0) );
	shadowMapWidth = 1024 * 8; //800 * 3;
	shadowMapHeight = 1024 * 8; //600 * 3;
	generateShadowFBO();

	/* main shader */
	main.setUniform("shadowMap", &shadowMapUniform);
	main.setUniform("DepthBiasMVP", &DepthBias);
	main.setUniform("LightPosition_worldspace", &Positions);

	/* depth shader */
	shadow.setUniform("depthMVP", &modelMatrix);

	/* texture translation matrix */
	biasMatrix = glm::mat4(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
	);

	t = 0.0;
}

LightingModel::~LightingModel() {
	// TODO Auto-generated destructor stub
}

void LightingModel::generateShadowFBO() {
	GLenum FBOstatus;

	// Try to use a texture depth component
	glGenTextures(1, &depthTextureId);
	glBindTexture(GL_TEXTURE_2D, depthTextureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadowMapWidth, shadowMapHeight, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	// create a framebuffer object
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	// Instruct openGL that we won't bind a color texture with the currently bound FBO
	glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	// attach the texture to FBO depth attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureId, 0);

	// check FBO status
	FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (FBOstatus != GL_FRAMEBUFFER_COMPLETE)
		printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");

	// switch back to window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightingModel::getDepthMap() {
	Positions.data.data()[0] = glm::vec4( 12.5f * sin(t), 8.0f, 12.5f * cos(t), 1.0 );
	t += 0.01;

	//First step: Render from the light POV to a FBO, story depth values only
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, fboId);	//Rendering offscreen
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void LightingModel::getShadow( shared_ptr<Geometry> g ) {

	// Compute the MVP matrix from the light's point of view
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-80,80,-80,80,-80,80);
	glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(Positions.data.data()[0]), glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat4 depthModelMatrix = g->transform();
	modelMatrix.setV( depthProjectionMatrix * depthViewMatrix * depthModelMatrix );
	g->draw();
}



void LightingModel::setLight() {
	// Now rendering from the camera POV, using the FBO to generate shadows
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Using the shadow shader
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, depthTextureId);
	shadowMapUniform.setV(7);
}

void LightingModel::setTransform(glm::mat4 t) {

	// Compute the MVP matrix from the light's point of view
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-80,80,-80,80,-80,80);
	glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(Positions.data.data()[0]), glm::vec3(0,0,0), glm::vec3(0,1,0));
	modelMatrix.setV(depthProjectionMatrix * depthViewMatrix * t);
	DepthBias.setV( biasMatrix * modelMatrix.getV() );
}

} /* namespace std */
