#ifndef UTIL_H_
#define UTIL_H_

#include <cfloat>
#include <map>
#include <string>
#include <sstream>

#include "OnlineVariance.h"

#define DEFAULT_TOLERANCE 0.001f

struct NormalDist {
	OnlineVariance normalStat;
	float min;
	float max;

	NormalDist() { min = FLT_MAX; max = -FLT_MAX; }

	void scale(float scale) {
		min *= scale;
		max *= scale;
		normalStat.scale(scale);
	}
};

struct DiscreteDist {
	std::map<int, float> values;
};

void anglesInRange(float &inclination, float &azimuth);

bool approxEqual(float a, float b, float tolerance = DEFAULT_TOLERANCE);
bool approxEqual(const std::pair<float, float> &a, const std::pair<float, float> &b, float tolerance = DEFAULT_TOLERANCE);
bool approxEqual(const NormalDist &a, float b, float tolerance = DEFAULT_TOLERANCE);
bool approxEqualRatio(float a, float b, float tolerance = DEFAULT_TOLERANCE);

bool atob(const char *node);

float average(float a, float b);
float average(std::pair<float, float> a);

void error(std::string message, bool fatal);

template<class T>
bool equal(std::vector<T> a, std::vector<T> b) {
	if (a.size() != b.size()) return false;
	for (size_t i = 0; i < a.size(); ++i) {
		if (a[i] != b[i]) return false;
	}
	return true;
}

template<class T>
std::string toString(const T &t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

#endif /* UTIL_H_ */
