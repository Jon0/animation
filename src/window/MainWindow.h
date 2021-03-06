/*
 * MainWindow.h
 *
 *  Created on: 25/08/2013
 *      Author: remnanjona
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <map>
#include <memory>
#include <vector>
#include <chrono>
#include <string>

#include <epoxy/gl.h>
#include <epoxy/glx.h>

#include "KeyListener.h"
#include "MouseListener.h"
#include "../view/ViewInterface.h"

namespace std {

class MainWindow:
		public enable_shared_from_this<MainWindow> {
public:
	GLuint g_mainWnd;
	MainWindow(int, int, string);
	virtual ~MainWindow();
	void addView(shared_ptr<ViewInterface> vi);
	virtual void start();
protected:
	int wnd_width, wnd_height;
	vector<shared_ptr<ViewInterface>> g_view;
	void display();
	void reshape(int, int);
	void keyboard(unsigned char, int, int);
	void mouseClick(int, int, int, int);
	void mouseDrag(int, int);
private:
	chrono::time_point<chrono::high_resolution_clock> time;
	static void displayCallback();
	static void reshapeCallback(int, int);
	static void keyboardCallback(unsigned char, int, int);
	static void mouseCallback(int button, int state, int x, int y);
	static void mouseCallbackMotionFunc(int x, int y);
	static void idleFunc();
};

extern map<int, shared_ptr<MainWindow>> instances;

} /* namespace std */
#endif /* MAINWINDOW_H_ */
