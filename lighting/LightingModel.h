/*
 * LightingModel.h
 *
 *  Created on: 25/09/2013
 *      Author: remnanjona
 */

#ifndef LIGHTING_MODEL_H_
#define LIGHTING_MODEL_H_

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/gl.h>

#include "../shader/Program.h"
#include "../shader/UniformControl.h"

namespace std {

struct LightProperties {
	glm::vec4 position;
	glm::vec4 color;
	float intensity;
};

class LightingModel {
public:
	int shadowMapWidth;
	int shadowMapHeight;
	unsigned int numLights;
	GLuint test;

	// Hold id of the framebuffer for light POV rendering
	vector<GLuint> fboId;

	// Z values will be rendered to this texture when using fboId framebuffer
	vector<GLuint> depthTextureId;

	/* matrix */
	glm::mat4 biasMatrix;

	/*
	 * uniforms
	 */
	UniformControl<glm::mat4> modelMatrix;

	UniformControl<vector<GLint>> shadowMaps;
	UniformControl<vector<glm::mat4>> DepthBias;

	UniformBlock<LightProperties> lightUniform;
	vector< UBO<LightProperties> > lights;

	float t;

	LightingModel(Program &shadow, Program &);
	virtual ~LightingModel();

	void generateShadowFBO();
	void getDepthMap();
	void getShadow( shared_ptr<Geometry> );
	void setLight();
	void setTransform(glm::mat4);
};

} /* namespace std */
#endif /* LIGHTING_MODEL_H_ */
