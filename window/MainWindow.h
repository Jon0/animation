/*
 * MainWindow.h
 *
 *  Created on: 25/08/2013
 *      Author: remnanjona
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <vector>
#include <chrono>
#include "KeyListener.h"
#include "MouseListener.h"
#include "../view/ViewInterface.h"

namespace std {

class MainWindow {
public:
	MainWindow(int, int);
	virtual ~MainWindow();
protected:
	GLuint g_mainWnd;
	int wnd_width, wnd_height;
	vector<ViewInterface *> g_view;
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

extern MainWindow *ins;

} /* namespace std */
#endif /* MAINWINDOW_H_ */
