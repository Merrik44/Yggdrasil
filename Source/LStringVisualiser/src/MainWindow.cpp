#include "MainWindow.h"

#include <cmath>
#include <iostream>
#include <GL/glut.h>

using namespace std;

#define FOV 70
#define MOUSE_WHEEL_UP 3
#define MOUSE_WHEEL_DOWN 4
#define WHEEL_SCROLL_AMOUNT 10.0f
#define CAMERA_PAN_AMOUNT 0.01f
#define DEFAULT_CAMERA_DISTANCE 600.0f

#define NEAR 0.1f
#define FAR 100000.0f

void clamp(float& val, float min, float max);

MainWindow *MainWindow::main = 0;

MainWindow::MainWindow(std::vector<BranchSegment *> *branches, int windowHandle) {
	if (MainWindow::main) {
		main->close();
	}
	MainWindow::main = this;

	this->windowHandle = windowHandle;

	this->branches = branches;

	leftMouseDown = false;

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glutDisplayFunc(MainWindow::displayMain);
	glutIdleFunc(MainWindow::idle);
	glutMouseFunc(MainWindow::mouseMain);
	glutMotionFunc(MainWindow::motionMain);
	glutPassiveMotionFunc(MainWindow::motionMain);
	glutReshapeFunc(MainWindow::reshapeMain);

	cameraMag = DEFAULT_CAMERA_DISTANCE;

	// Aim the camera down the z-axis
	cameraTheta = 0;
	cameraPhi = M_PI / 2;

	glutMainLoop();
}

MainWindow::~MainWindow() {
	close();
}

void MainWindow::close() {
	if (MainWindow::main == this) {
		MainWindow::main = 0;
	}
}

void MainWindow::display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-FAR, 0, 0);
	glVertex3f(FAR, 0, 0);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(0, -FAR, 0);
	glVertex3f(0, FAR, 0);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(0, 0, -FAR);
	glVertex3f(0, 0, FAR);
	glEnd();

	// Draw each object
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);
	glEnd();
	for (vector<BranchSegment *>::size_type i = 0; i != branches->size(); ++i) {
		branches->at(i)->draw();
	}

	glutSwapBuffers();
}

void MainWindow::handleMouseInput(int button, int state, int x, int y) {
	if (state == GLUT_UP) {
		// Zoom in or out, but don't zoom in too far
		if (button == MOUSE_WHEEL_UP || button == MOUSE_WHEEL_DOWN) {
			int direction;
			if (button == MOUSE_WHEEL_UP) {
				direction = -1;
			} else {
				direction = 1;
			}
			float desired = cameraMag + WHEEL_SCROLL_AMOUNT * direction;
			if (desired >= NEAR) {
				cameraMag = desired;
				setCamera();
			}
		} else if (button == GLUT_LEFT_BUTTON) {
			leftMouseDown = false;
		}
	} else if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			leftMouseDown = true;
		}
	}
}

void MainWindow::handleMouseMotion(int x, int y) {
	bool changed = (x != mouseX) || (y != mouseY);

	if (changed) {
		int changeX = x - mouseX;
		int changeY = y - mouseY;

		// Pan the camera, but keep the angles within bounds
		if (leftMouseDown) {
			cameraTheta -= changeX * CAMERA_PAN_AMOUNT;
			while (cameraTheta > 2 * M_PI) {
				cameraTheta -= 2 * M_PI;
			}
			while (cameraTheta < 0) {
				cameraTheta += 2 * M_PI;
			}
			cameraPhi -= changeY * CAMERA_PAN_AMOUNT;
			clamp(cameraPhi, CAMERA_PAN_AMOUNT, M_PI - CAMERA_PAN_AMOUNT);

			setCamera();
		}
	}

	mouseX = x;
	mouseY = y;
}

void MainWindow::reshape(int width, int height) {
	if (width == 0 || height == 0) {
		return;
	}

	// Redo the viewport and perspective, recalculating the aspect ratio
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(FOV, width / (float)height, NEAR, FAR);

	setCamera();
}

void MainWindow::setCamera() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Convert spherical coordinates
	float cameraX = cameraMag * sin(cameraTheta) * sin(cameraPhi);
	float cameraY = cameraMag * cos(cameraPhi);
	float cameraZ = cameraMag * cos(cameraTheta) * sin(cameraPhi);
	gluLookAt(cameraX, cameraY, cameraZ, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void MainWindow::displayMain() {
	if (MainWindow::main) {
		MainWindow::main->display();
	}
}

void MainWindow::motionMain(int x, int y) {
	if (MainWindow::main) {
		MainWindow::main->handleMouseMotion(x, y);
	}
}

void MainWindow::mouseMain(int button, int state, int x, int y) {
	if (MainWindow::main) {
		MainWindow::main->handleMouseInput(button, state, x, y);
	}
}

void MainWindow::idle() {
	glutPostRedisplay();
}

void MainWindow::reshapeMain(int width, int height) {
	if (MainWindow::main) {
		MainWindow::main->reshape(width, height);
	}
}

void clamp(float& val, float min, float max) {
	if (val < min) {
		val = min;
	} else if (val > max) {
		val = max;
	}
}
