/*
 * Shader.h
 *
 *  Created on: 20/09/2013
 *      Author: remnanjona
 */

#ifndef SHADER_H_
#define SHADER_H_

#include <epoxy/gl.h>
#include <epoxy/glx.h>

namespace std {

class Shader {
public:
	GLuint ShaderHandle;
	Shader(string, GLenum);
	virtual ~Shader();
};

} /* namespace std */
#endif /* SHADER_H_ */
