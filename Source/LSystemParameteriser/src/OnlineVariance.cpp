#include <cmath>

#include "OnlineVariance.h"

OnlineVariance::OnlineVariance() {
	clearValues();
}

OnlineVariance::~OnlineVariance() {
}

void OnlineVariance::addValue(float value) {
	++numValues;

	if (numValues == 1) {
		oldMean = value;
		newMean = value;
		oldDev = 0.0f;
	} else {
		newMean = oldMean + (value - oldMean) / numValues;
		newDev = oldDev + (value - oldMean) * (value - newMean);

		oldMean = newMean;
		oldDev = newDev;
	}
}

void OnlineVariance::clearValues() {
	numValues = 0;
}

int OnlineVariance::getNumValues() const {
	return numValues;
}

double OnlineVariance::getMean() const {
	if (numValues > 0) return newMean;
	else return 0.0f;
}

double OnlineVariance::getVariance() const {
	if (numValues > 1) return newDev / (numValues - 1);
	else return 0.0f;
}

double OnlineVariance::getStandardDeviation() const {
	return sqrtf(getVariance());
}

void OnlineVariance::scale(float scale) {
	newDev *= scale * scale;
	oldDev *= scale * scale;
	newMean *= scale;
	oldMean *= scale;
}
