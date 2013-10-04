/*
 * GRender.cpp
 *
 *  Created on: 23/09/2013
 *      Author: remnanjona
 */

#include <iostream>
#include "../geometry/Cube.h"
#include "../texture/Font.h"
#include "GRender.h"

namespace std {

GRender::GRender():
		mWnd { new MainWindow(800, 600, "Scene") },
		program("phong_bump"),
		shadow("shadow_depth"),
		skybox("skybox"),
		vb(15),
		gloader(),
		sky { new Cube(500) },
		box { gloader.readOBJG("assets/obj/Box.obj") },
		bunny { gloader.readOBJG("assets/obj/Bunny.obj") },
		sphere { gloader.readOBJG("assets/obj/Sphere.obj") },
		table { gloader.readOBJG("assets/obj/Table.obj") },
		teapot { gloader.readOBJG("assets/obj/Teapot.obj") },
		torus { gloader.readOBJG("assets/obj/Torus.obj") },
		light { shadow, program },
		materialUniform { program.getBlock<MaterialProperties>("MaterialProperties", 1) }
{
	mWnd->start();

	/* texturing... */
	woodTex = new Tex();
	woodTex->make2DTex("assets/image/Burled Cherry_DIFFUSE.jpg");
	woodNormTex = new Tex();
	woodNormTex->make2DTex("assets/image/Burled Cherry_NORMAL.jpg");
	woodDispTex = new Tex();
	woodDispTex->make2DTex("assets/image/Burled Cherry_DISP.jpg");

	brickTex = new Tex();
	brickTex->make2DTex("assets/image/brickdiff.jpg");
	brickNormTex = new Tex();
	brickNormTex->make2DTex("assets/image/bricknorm.jpg");

	normalTex = new Tex();
	normalTex->make2DTex("assets/image/normal.jpg");
	cubeTex = new Tex();
	cubeTex->make3DTex("assets/image/sky2.png");

	useDiffTex = program.addUniform("useDiffTex");
	useNormTex = program.addUniform("useNormTex");

	// setup VBO
	sky->init(&vb);
	box->init(&vb);
	bunny->init(&vb);
	sphere->init(&vb);
	table->init(&vb);
	teapot->init(&vb);
	torus->init(&vb);
	vb.store();

	table->setTransform( glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0)) );
	box->setTransform( glm::translate(glm::mat4(1.0), glm::vec3(5.0, 2.5, 6.0)) );
	bunny->setTransform( glm::translate(glm::mat4(1.0), glm::vec3(0,0.5,0)) );
	sphere->setTransform( glm::translate(glm::mat4(1.0), glm::vec3(4,2.0,-7)) );
	teapot->setTransform( glm::translate(glm::mat4(1.0), glm::vec3(-4,0.5,-7)) );
	torus->setTransform( glm::translate(glm::mat4(1.0), glm::vec3(-6,1,5)) );

	MaterialProperties &mpb = bunny->material();
	mpb.AmbientColor = glm::vec4(0.0, 0.0, 0.0, 0.5);
	mpb.DiffuseColor = glm::vec4(0.588235, 0.670588, 0.729412, 0.5);
	mpb.SpecularColor = glm::vec4(0.9, 0.9, 0.9, 0.5);
	mpb.Exponent = 96.0;
	bunny->updateMaterial();

	MaterialProperties &mptp = teapot->material();
	mptp.AmbientColor = glm::vec4(0.105882, 0.058824, 0.113725, 1.0);
	mptp.DiffuseColor = glm::vec4(0.427451, 0.470588, 0.541176, 1.0);
	mptp.SpecularColor = glm::vec4(0.333333, 0.333333, 0.521569, 1.0);
	mptp.Exponent = 9.84615;
	teapot->updateMaterial();

	MaterialProperties &mps = sphere->material();
	mps.AmbientColor = glm::vec4(0.2125, 0.1275, 0.054, 1.0);
	mps.DiffuseColor = glm::vec4(0.714, 0.4284, 0.18144, 1.0);
	mps.SpecularColor = glm::vec4(0.393548, 0.271906, 0.166721, 1.0);
	mps.Exponent = 25.6;
	sphere->updateMaterial();

	MaterialProperties &mpt = torus->material();
	mpt.AmbientColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
	mpt.DiffuseColor = glm::vec4(0.85, 0.05, 0.05, 1.0);
	mpt.SpecularColor = glm::vec4(0.9, 0.7, 0.7, 1.0);
	mpt.Exponent = 200.0;
	torus->updateMaterial();

	selectedLight = 1;
	camptr = NULL;
	t = 0.0;

	lightcontrol = 0;
	message = "Position";
	showIcons = true;

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

GRender::~GRender() {
	// TODO Auto-generated destructor stub
}

void GRender::start() {
	camera = shared_ptr<Camera>{ new Camera( shared_from_this(), mWnd ) };
	mWnd->addView( camera );

	ortho = shared_ptr<Ortho>{ new Ortho( shared_from_this(), mWnd ) };
	mWnd->addView( ortho );

	/*
	 * set uniforms
	 */
	UniformBlock<CameraProperties> camsky = skybox.getBlock<CameraProperties>( "Camera", 1 );
	camsky.assign( &camera->properties, 0 );

	skybox.setUniform("cubeTexture", &cubeTex->location);

	program.setUniform("cubeTexture", &cubeTex->location);
	program.setUniform("diffuseTexture", &brickTex->location);
	program.setUniform("normalTexture", &normalTex->location);
	program.setUniform("specularTexture", &woodDispTex->location);

	UniformBlock<CameraProperties> cam = program.getBlock<CameraProperties>( "Camera", 1 );
	cam.assign( &camera->properties, 0 );
	camptr = &camera->properties;
}

/*
 * TODO only call this once
 */
void GRender::prepare() {
	LightProperties &p = light.getLight(0);
	p.position = glm::vec4( 12.5f * sin(t), 8.0f, 12.5f * cos(t), 1.0 );
	light.updateLight(0);
	t += 0.01;

	shadow.enable();

	/*
	 * prepare depth map of each light
	 */
	light.clearDepthMap();
	light.createShadow(table);
	light.createShadow(box);
	light.createShadow(bunny);
	light.createShadow(sphere);
	light.createShadow(teapot);
	light.createShadow(torus);
}

void GRender::display( shared_ptr<ViewInterface> cam, chrono::duration<double> ) {
	cubeTex->enable(0);

	/* TODO translate by camera position */
	glDisable(GL_CULL_FACE);
	skybox.enable();
	vb.enable();
	sky->draw();

	/*
	 * Draw the scene objects
	 */
	brickTex->enable(0);
	normalTex->enable(1);
	woodDispTex->enable(2);
	cubeTex->enable(3);

	program.enable();
	glEnable(GL_CULL_FACE);

	light.setLight();
	displayGeometry();

	if (showIcons) light.drawIcons();
}

void GRender::displayUI() {
	drawString( message, 10, 10 );
}

void GRender::displayGeometry() {
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	vb.enable();

	glUniform1i(useDiffTex, true);
	glUniform1i(useNormTex, true);
	woodTex->enable(0);
	woodNormTex->enable(1);
	woodDispTex->enable(2);
	drawObject(table);

	brickTex->enable(0);
	brickNormTex->enable(1);
	brickTex->enable(2);
	drawObject(box);

	glUniform1i(useDiffTex, false);
	normalTex->enable(1);
	drawObject(torus);

	glUniform1i(useNormTex, false);
	drawObject(sphere);
	drawObject(teapot);

	/* bunny last, it has transperency */
	drawObject(bunny);
}

void GRender::drawObject( shared_ptr<Geometry> g ) {
	materialUniform.assign(g->materialUBO());

	light.setTransform(g->transform());
	camptr->data.M = g->transform();
	camptr->update();
	g->draw();
}

int GRender::mouseClicked( shared_ptr<ViewInterface> cam, int button, int state, int x, int y ) {
	if (state) {
		drag = false;
		return false;
	}

	if (button == 0) {
		glm::vec3 p = cam->project(glm::vec3(0, 0, 0));
		getArc(p.x, p.y, x, y, 600.0, click_old);
		click_old = glm::inverse(cam->cameraAngle()) * click_old * cam->cameraAngle();
		drag = true;
		return true;
	}

	LightProperties &l = light.getLight(selectedLight);
	glm::vec4 *var;
	if (lightcontrol == 0) {
		var = &l.position;
	} else if (lightcontrol == 1) {
		var = &l.direction;
	} else if (lightcontrol == 2) {
		var = &l.color;
	}
	if (button == 3) {
		var->x *= 1.05;
		var->y *= 1.05;
		var->z *= 1.05;
	} else if (button == 4) {
		var->x /= 1.05;
		var->y /= 1.05;
		var->z /= 1.05;
	}
	light.updateLight(selectedLight);
	return true;

}

int GRender::mouseDragged(shared_ptr<ViewInterface> cam, int x, int y) {
	if (drag && selectedLight >= 0) {
		LightProperties &l = light.getLight(selectedLight);
		glm::vec3 p = cam->project( glm::vec3(0,0,0) );

		// modify orientation
		getArc(p.x, p.y, x, y, 600.0, click_new);
		click_new = glm::inverse(cam->cameraAngle()) * click_new * cam->cameraAngle();
		glm::quat drag = click_new * glm::inverse(click_old);
		if (lightcontrol == 0) {
			l.position = drag * l.position;
		}
		else if (lightcontrol == 1) {
			l.direction = drag * l.direction;
		}
		else if (lightcontrol == 2) {
			l.color = drag * l.color;
			l.color.x = abs(l.color.x);
			l.color.y = abs(l.color.y);
			l.color.z = abs(l.color.z);
		}

		light.updateLight(selectedLight);
		click_old = click_new;
		return true;
	}

	return false;
}

void GRender::messageSent(string) {

}

void GRender::keyPressed(unsigned char c) {
	LightProperties &l = light.getLight(selectedLight);
	switch (c) {
	case 'a':
		lightcontrol = 0;
		message = "Position";
		break;
	case 's':
		lightcontrol = 1;
		message = "Spot";
		break;
	case 'd':
		lightcontrol = 2;
		message = "Color";
		break;
	case '.':
		showIcons = !showIcons;
		break;
	}
}

} /* namespace std */
