#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <vector>

#include "BranchSegment.h"

class MainWindow {
public:
	MainWindow(std::vector<BranchSegment *> *branches, int windowHandle);
	virtual ~MainWindow();

	void close();
	void display();
	void handleMouseInput(int button, int state, int x, int y);
	void handleMouseMotion(int x, int y);
	void reshape(int width, int height);

	static void displayMain();
	static void motionMain(int x, int y);
	static void mouseMain(int button, int state, int x, int y);
	static void idle();
	static void reshapeMain(int width, int height);

	static MainWindow *main;
private:
	void setCamera();

	std::vector<BranchSegment *> *branches;

	float cameraMag;
	float cameraTheta;
	float cameraPhi;

	int mouseX;
	int mouseY;
	bool leftMouseDown;

	int windowHandle;
};

#endif /* MAINWINDOW_H_ */
