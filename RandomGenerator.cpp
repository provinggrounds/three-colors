#include "RandomGenerator.h"
#include <cmath>
#include <ctime>
void RandomGenerator::initialize(void)
	{
		this->min=gen.min();
		this->max=gen.max();
	}
void RandomGenerator::seed(int s)
	{
		this->gen.seed(s);
	}
RandomGenerator::RandomGenerator()
	{
		this->initialize();
		this->seed(std::time(NULL));
	}
RandomGenerator::RandomGenerator(int Seed)
	{
		this->initialize();
		this->seed(Seed);
	}
double RandomGenerator::RandomDouble(void)
	{
		return (double)(gen()-min)/((double)(max-min)+1);
	}
void GetRandomVector(int Dimension, RandomGenerator & gen, double * result)
{
	double r;
	do
	{
		r=0;
		for(int i=0; i<Dimension; i++)
		{
			result[i]=2*gen.RandomDouble()-1;
			r+=result[i]*result[i];
		}
	}
	while(r>1);
	for(int i=0; i<Dimension; i++)
		result[i]/=std::sqrt(r);

	return;
}
