/*
 * GRender.h
 *
 *  Created on: 23/09/2013
 *      Author: remnanjona
 */

#ifndef GRENDER_H_
#define GRENDER_H_

#include <memory>
#include <vector>
#include "SceneInterface.h"
#include "../geometry/Geometry.h"
#include "../lighting/LightingModel.h"
#include "../load/GeometryLoader.h"
#include "../shader/Program.h"
#include "../view/Camera.h"
#include "../window/MainWindow.h"

namespace std {

class GRender:
		public enable_shared_from_this<GRender>,
		public SceneInterface {
public:
	shared_ptr<MainWindow> mWnd;
	Program program;
	Program shadow;
	Program skybox;
	VertexBuffer vb;
	GeometryLoader gloader;
	shared_ptr<Camera> camera;
	shared_ptr<Geometry> sky;
	shared_ptr<Geometry> box;
	shared_ptr<Geometry> bunny;
	shared_ptr<Geometry> sphere;
	shared_ptr<Geometry> table;
	shared_ptr<Geometry> teapot;
	shared_ptr<Geometry> torus;
	LightingModel light;

	UniformControl<glm::mat4> model;

	GLuint useDiffTex, useNormTex;

	/* Textures */
	Tex *woodTex;
	Tex *brickTex;
	Tex *normalTex;
	Tex *cubeTex;

	UBO<CameraProperties> *camptr; // TODO this is a hack

	GRender();
	virtual ~GRender();

	void start();
	void drawObject( shared_ptr<Geometry> );
	virtual void prepare();
	virtual void display( shared_ptr<ViewInterface>, chrono::duration<double> );
	virtual void displayGeometry();
	virtual int mouseClicked( shared_ptr<ViewInterface>, int, int, int, int );
	virtual int mouseDragged( shared_ptr<ViewInterface>, int, int );
	virtual void messageSent(string);
	virtual void keyPressed(unsigned char);
};

} /* namespace std */
#endif /* GRENDER_H_ */
