#include <ctime>
#include <iostream>

#include "Random.h"

Random::Random() {
}

Random::~Random() {
}

boost::mt19937 Random::rng;
boost::uniform_real<float> Random::uniform_real;

void Random::init() {
	rng.seed(static_cast<unsigned int>(std::time(0)));
}

float Random::normal(float mean, float stddev) {
	boost::normal_distribution<float> norm(mean, stddev);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<float> > var(rng, norm);

	return var();
}

float Random::uniform(float min, float max) {
	if (min > max) return Random::uniform(max, min);

	return  min + (max - min) * uniform_real(rng);
}
