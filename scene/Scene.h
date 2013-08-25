/*
 * Scene.h
 *
 *  Created on: 25/08/2013
 *      Author: remnanjona
 */

#ifndef SCENE_H_
#define SCENE_H_

#include "SceneInterface.h"
#include "../geometry/Skeleton.h"

namespace std {

class Scene: public SceneInterface {
public:
	Scene();
	virtual void display();
	virtual ~Scene();
protected:
	Skeleton *skeleton;
};

} /* namespace std */
#endif /* SCENE_H_ */
