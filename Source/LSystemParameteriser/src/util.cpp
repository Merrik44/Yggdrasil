#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "util.h"

using namespace std;

void anglesInRange(float &inclination, float &azimuth) {
	// Get inclination into range [-Pi, Pi)
	while (inclination < -M_PI) inclination += 2.0f * M_PI;
	while (inclination >= M_PI) inclination -= 2.0f * M_PI;
	// Get azimuth into range [0, 2Pi)
	while (azimuth < 0.0f) azimuth += 2.0f * M_PI;
	while (azimuth >= 2.0f * M_PI) azimuth -= 2.0f * M_PI;

	// We actually want the azimuth in range [0, Pi)
	if (azimuth >= M_PI) {
		azimuth -= M_PI;
		inclination *= -1;
		if (inclination == M_PI) inclination = -M_PI;
	}
}

float average(float a, float b) {
	return (a + b) / 2.0f;
}

float average(std::pair<float, float> a) {
	return average(a.first, a.second);
}

bool approxEqual(float a, float b, float tolerance) {
	return b - tolerance <= a && a <= b + tolerance;
}

bool approxEqual(const pair<float, float> &a, const pair<float, float> &b, float tolerance) {
	return approxEqual(a.first, b.first, tolerance) && approxEqual(a.second, b.second, tolerance);
}

bool approxEqual(const NormalDist &a, float b, float tolerance) {
	return approxEqual(a.min, b, tolerance) && approxEqual(a.max, b, tolerance);
}

bool approxEqualRatio(float a, float b, float tolerance) {
	return b * (1 - tolerance) <= a && a <= b * (1 + tolerance);
}

bool atob(const char *node) {
	size_t len = strlen(node);
	char *temp = new char[len + 1];
	strcpy(temp, node);

	for (size_t i = 0; i < len; ++i) temp[i] = tolower(temp[i]);

	bool val;
	if (strncmp(temp, "true", len) == 0) val = true;
	else val = false;

	delete [] temp;

	return val;
}

void error(std::string message, bool fatal) {
	cerr << "Error: " << message << endl;

	if (fatal) exit(1);
}
