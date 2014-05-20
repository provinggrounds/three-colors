#include "StealthyChecker.h"
#include "GeometryVector.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

//const double pi=3.1415926535897932384626433832795028841971693993751058209;

StealthyChecker::StealthyChecker(const std::vector<int> & sizes, const std::vector<int> & KConstraint)
{
	DimensionType dim=sizes.size();
	if(dim>= ::MaxDimension)
	{
		std::cerr<<"Error in StealthyChecker::StealthyChecker : system dimension is greater than MaxDimension!\n";
		std::cerr<<"Please increase MaxDimension in GeometryVector.h!\n";
		std::exit(1);
	}
	if(dim!=KConstraint.size())
	{
		std::cerr<<"Error in StealthyChecker::StealthyChecker : The dimensions of system size and K constraint do not agree!\n";
		std::exit(1);
	}
	GeometryVector k(dim);
	for(size_t i=0; i<KConstraint.size(); i++)
		k.x[i]=2*pi*KConstraint[i];
	this->SumCoskr=0;
	this->SumSinkr=0;

	std::vector<int> now = sizes;
	while(now.back()!=0)
	{
		GeometryVector r(dim);
		for(size_t i=0; i<now.size(); i++)
			r.x[i]=static_cast<double>(now[i])/sizes[i];

		double kr=k.Dot(r);

		this->coskr.push_back(std::cos(kr));
		this->SumCoskr+=std::cos(kr);
		this->sinkr.push_back(std::sin(kr));
		this->SumSinkr+=std::sin(kr);

		now[0]--;
		for(size_t i=0; i<now.size()-1; i++)
		{
			if(now[i]==0)
			{
				now[i]=sizes[i];
				now[i+1]--;
			}
		}
	}
}

//int debug()
//{
//	std::vector<int> sizes;
//	sizes.push_back(3);
//	sizes.push_back(3);
//	std::vector<int> k;
//	k.push_back(0);
//	k.push_back(1);
//	StealthyChecker checker(sizes, k);
//
//	std::vector<signed char> conf;
//	conf.push_back(1);
//	conf.push_back(1);
//	conf.push_back(-1);
//	conf.push_back(-1);
//	conf.push_back(1);
//	conf.push_back(1);
//	conf.push_back(1);
//	conf.push_back(-1);
//	conf.push_back(1);
//
//	checker.IsStealthy(conf);
//
//	return 0;
//}
//
//int main()
//{
//	return debug();
//}
