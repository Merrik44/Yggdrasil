#ifndef RANDOM_H_
#define RANDOM_H_

#include <boost/random.hpp>

class Random {
public:
	virtual ~Random();

	static void init();

	static float normal(float mean, float stddev);
	static float uniform(float min, float max);

private:
	Random();

	static boost::mt19937 rng;
	static boost::uniform_real<float> uniform_real;
};

#endif /* RANDOM_H_ */
