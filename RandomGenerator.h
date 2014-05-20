#ifndef RANDOMGENERATOR_INCLUDED
#define RANDOMGENERATOR_INCLUDED
#include <boost/random/mersenne_twister.hpp>
class RandomGenerator
{
private:
	boost::mt19937 gen;
	boost::mt19937::result_type min, max;
	void initialize(void);
public:
	void seed(int s);
	RandomGenerator();
	RandomGenerator(int Seed);
	double RandomDouble(void);
};
void GetRandomVector(int Dimension, RandomGenerator & gen, double * result);

#endif
