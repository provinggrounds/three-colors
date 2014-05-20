#ifndef STEALTHYCHECKER_INCLUDED
#define STEALTHYCHECKER_INCLUDED

#include <vector>
#include <cassert>
#include <cmath>

const double StealthyTolerence = 1e-10;
class StealthyChecker
{
public:
	std::vector<double> coskr, sinkr;
	double SumCoskr, SumSinkr;
	StealthyChecker(const std::vector<int> & sizes, const std::vector<int> & KConstraint);

	double GetRhoR(const std::vector<signed char> & conf) const
	{
		double rhor=0;
		std::vector<double>::const_iterator IterCoskr=coskr.begin();
		std::vector<signed char>::const_iterator Iterconf=conf.begin();
		for(; Iterconf!=conf.end(); Iterconf++, IterCoskr++)
		{
			rhor+=(*Iterconf)*(*IterCoskr);
		}
		return rhor;
	}
	double GetRhoI(const std::vector<signed char> & conf) const
	{
		double rhoi=0;
		std::vector<double>::const_iterator IterSinkr=sinkr.begin();
		std::vector<signed char>::const_iterator Iterconf=conf.begin();
		for(; Iterconf!=conf.end(); Iterconf++, IterSinkr++)
		{
			rhoi+=(*Iterconf)*(*IterSinkr);
		}
		return rhoi;
	}
	double GetRhoModulusSquared_Plus_Minus(const std::vector<signed char> & conf) const
	{
		assert(conf.size()==this->coskr.size());
		assert(conf.size()==this->sinkr.size());
		double rhor=this->GetRhoR(conf), rhoi=this->GetRhoI(conf);
		return rhor*rhor+rhoi*rhoi;
	}
	double GetRhoModulusSquared_Particle_Void(const std::vector<signed char> & conf) const
	{
		assert(conf.size()==this->coskr.size());
		assert(conf.size()==this->sinkr.size());
		double rhor=this->GetRhoR(conf), rhoi=this->GetRhoI(conf);
		rhor=(rhor+SumCoskr)/2.0;
		rhoi=(rhoi+SumSinkr)/2.0;
		return rhor*rhor+rhoi*rhoi;
	}

	bool IsStealthy(const std::vector<signed char> & conf)
	{
		return ( this->GetRhoModulusSquared_Plus_Minus(conf) < 4* ::StealthyTolerence );
	}
};

#endif