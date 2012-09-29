#ifndef ONLINEVARIANCE_H_
#define ONLINEVARIANCE_H_

// Calculate variance in an on-line fashion, using Welford's algorithm, one data value at a time.
// See: http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#On-line_algorithm
class OnlineVariance {
public:
	OnlineVariance();
	virtual ~OnlineVariance();

	void addValue(float value);
	void clearValues();

	int getNumValues() const;
	double getMean() const;
	double getVariance() const;
	double getStandardDeviation() const;

	void scale(float scale);

private:
	int numValues;

	float oldMean, newMean;
	float oldDev, newDev;
};

#endif /* ONLINEVARIANCE_H_ */
