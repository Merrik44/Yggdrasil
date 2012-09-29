#include <cmath>
#include <GL/glut.h>
#include <iostream>
#include <vector>

#include "BranchSegment.h"
#include "MainWindow.h"

using namespace std;

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

int main(int argc, char **argv) {
	if (!cin) return 1;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glutInitWindowPosition(0, 0);

	int windowHandle = glutCreateWindow("L-system");

	istream *stream = &cin;

	vector<BranchSegment*> *branches = new vector<BranchSegment*>;

	string line;
	bool cut = false;

	float matrix[16];
	for (int i = 0; i < 16; ++i) {
		matrix[i] = 0.0f;
	}

	getline(*stream, line);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	while (getline(*stream, line)) {
		while (line.size() > 0 && (line[0] == ' ' || line[0] == '\t')) {
			line.assign(line.begin() + 1, line.end());
		}
		if (line == "") continue;

		if (line == "EB()") {
			glPopMatrix();
			cut = false;
			continue;
		}

		if (cut) continue;

		if (line == "SB()") {
			glPushMatrix();
		} else if (line == "Cut()") {
			cut = true;
		} else if (line.find("MovRel3f") != string::npos) {
			size_t lpos = line.find('(');
			size_t comma1pos = line.find(',', lpos + 1);
			size_t comma2pos = line.find(',', comma1pos + 1);
			size_t rpos = line.find(')');

			float x = atof(line.substr(lpos + 1, comma1pos - lpos - 1).c_str());
			float y = atof(line.substr(comma1pos + 1, comma2pos - comma1pos - 1).c_str());
			float z = atof(line.substr(comma2pos + 1, rpos - comma2pos - 1).c_str());

			glTranslatef(x, y, z);
		} else if (line.find("RotRel3f") != string::npos) {
			size_t lpos = line.find('(');
			size_t comma1pos = line.find(',', lpos + 1);
			size_t comma2pos = line.find(',', comma1pos + 1);
			size_t comma3pos = line.find(',', comma2pos + 1);
			size_t rpos = line.find(')');

			float angle = atof(line.substr(lpos + 1, comma1pos - lpos - 1).c_str());
			float x = atof(line.substr(comma1pos + 1, comma2pos - comma1pos - 1).c_str());
			float y = atof(line.substr(comma2pos + 1, comma3pos - comma2pos - 1).c_str());
			float z = atof(line.substr(comma3pos + 1, rpos - comma3pos - 1).c_str());

			angle *= 180.0f / M_PI;
			glRotatef(angle, x, y, z);
		} else if (line.find("Cylinder") != string::npos) {
			size_t lpos = line.find('(');
			size_t comma1pos = line.find(',', lpos + 1);
			size_t comma2pos = line.find(',', comma1pos + 1);
			size_t rpos = line.find(')');

			float baseRadius = atof(line.substr(lpos + 1, comma1pos - lpos - 1).c_str());
			float topRadius = atof(line.substr(comma1pos + 1, comma2pos - comma1pos - 1).c_str());
			float height = atof(line.substr(comma2pos + 1, rpos - comma2pos - 1).c_str());

			glFlush();
			glFinish();
			glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
			BranchSegment *branch = new BranchSegment(matrix, baseRadius, topRadius, height);
			branches->push_back(branch);
		} else {
			//cout << "Unrecognised: " << line << endl;
		}
	}

	MainWindow mainWindow(branches, windowHandle);

	if (windowHandle > 0) {
		glutDestroyWindow(windowHandle);
		windowHandle = 0;
	}

	for (vector<BranchSegment *>::size_type i = 0; i < branches->size(); ++i) {
		delete branches->at(i);
	}
	delete branches;

	return 0;
}
