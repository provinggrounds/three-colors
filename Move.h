#ifndef MCMOVE_INCLUDED
#define MCMOVE_INCLUDED

#include <vector>
#include "StealthyChecker.h"
#include "RandomGenerator.h"


const size_t TrialCount_SampleSize = 1000000;
const double MinAcceptanceRate = 0.1;
//get square lattice point within radius that are not inverse of each other
inline std::vector<std::vector<int> > GetSquareLatticePoint (double radius)
{
	std::vector<std::vector<int> > result;
	for(size_t i=0; i<radius; i++)
	{
		for(size_t j=0; j<radius; j++)
		{
			if( j*j+i*i>radius*radius)
				break;
			if(i==0 && j==0)
				continue;
			std::vector<int> temp(2, 0);
			temp[0]=i;
			temp[1]=j;
			result.push_back(temp);
			if(i!=0 && j!=0)
			{
				std::vector<int> temp(2, 0);
				temp[0]=i;
				temp[1]=(-1)*j;
				result.push_back(temp);
			}
		}
	}
	return result;
}




// E = sum_k S_0(k)^2 + S_1(k)^2
// S(k) = |rho(k)|^2 = rho_R^2 + rho_I^2
double Energy(std::vector<signed char> & sys0, std::vector<signed char> & sys1, const std::vector<StealthyChecker> & KPoints, size_t NbrSpin)
{

    double result = 0, temp = 0;

	for(auto iter=KPoints.begin(); iter!=KPoints.end(); iter++)
	{
        temp = iter->GetRhoR(sys0);
		result+=temp*temp;
        
        temp = iter->GetRhoI(sys0);
        result+=temp*temp;
        
        temp = iter->GetRhoR(sys1);
        result+=temp*temp;
        
        temp = iter->GetRhoI(sys1);
        result+=temp*temp;

	}
    
	return result/NbrSpin;
}

class SpinExchangeMove 
{
public:
	int Size0;//horizontal size of the system
	size_t NbrNeighbor;
	std::vector<size_t> NeighborList;//NeighborList[ n*NbrNeighbor + m ] is the mth neighbor of the nth spin
    
	size_t TrialCount, AcceptCount;
	double NeighborListRadius;
    
	size_t NbrSpin;
	size_t m, n;
	std::vector<double> RhoRs0, RhoIs0, RhoRs1, RhoIs1;
	std::vector<double> NextRhoRs0, NextRhoIs0, NextRhoRs1, NextRhoIs1;//this will be the new rho's if accept the move
    
public:
	bool GlobalMove;
    
    
	void GenerateNeighborList(double Radius)
	{
		//debug temp
		//std::cout<<"Neibor List Radius adjusted to"<<Radius<<"\n";
        
		std::vector<std::vector<int> > displacement = GetSquareLatticePoint(Radius);
		this->NbrNeighbor = displacement.size();
		this->NeighborList.resize(this->NbrNeighbor*this->NbrSpin);
        
		for(size_t NSpin =0; NSpin<this->NbrSpin; NSpin++)
		{
			for(size_t NNeighbor =0; NNeighbor< this->NbrNeighbor; NNeighbor++)
			{
				//size_t temp=NSpin+displacement[NNeighbor][0]+displacement[NNeighbor][1]*Size0;
				int tx=NSpin%Size0;
				int ty=NSpin/Size0;
				tx+=displacement[NNeighbor][0];
				while(tx<0)
					tx+=Size0;
				while(tx>=Size0)
					tx-=Size0;
				ty+=displacement[NNeighbor][1];
				double temp = ty*Size0+tx;
                
				while(temp<0)
					temp+=NbrSpin;
				while(temp>=NbrSpin)
					temp-=NbrSpin;
				this->NeighborList[ NSpin*NbrNeighbor + NNeighbor ] = temp;
			}
		}
		this->NeighborListRadius=Radius;
	}


	SpinExchangeMove(const std::vector<signed char> & Configuration, const std::vector<signed char> & conf0, const std::vector<signed char> & conf1, const std::vector<StealthyChecker> & KPoints)
	{
		this->NbrSpin=Configuration.size();
		this->m=0;
		this->n=0;
		for(std::vector<StealthyChecker>::const_iterator iter=KPoints.begin(); iter!=KPoints.end(); iter++)
		{
			RhoRs0.push_back(iter->GetRhoR(conf0));
			RhoIs0.push_back(iter->GetRhoI(conf0));
			RhoRs1.push_back(iter->GetRhoR(conf1));
			RhoIs1.push_back(iter->GetRhoI(conf1));
		}
		NextRhoIs0.resize(KPoints.size());
		NextRhoRs0.resize(KPoints.size());
		NextRhoIs1.resize(KPoints.size());
		NextRhoRs1.resize(KPoints.size());
	}
	//Generate a random move, return Energy difference. the PreFactor is also set if it is not 1.
	double DeltaEnergy(std::vector<signed char> & sys, std::vector<signed char> & sys0, std::vector<signed char> & sys1, const std::vector<StealthyChecker> & KPoints, RandomGenerator & gen, double & PreFactor, bool LockStepSize)
	{
		do
		{
			this->m = std::floor(gen.RandomDouble() * NbrSpin);
			this->n = std::floor(gen.RandomDouble() * NbrSpin);
		}
		while(sys[m] == sys[n]);

        double Eold=0.0, Enew=0.0;
        
		for(size_t i=0; i<KPoints.size(); i++)
		{

            NextRhoRs0[i]=RhoRs0[i]+( sys0[m] - sys0[n])*(KPoints[i].coskr[n] - KPoints[i].coskr[m] );
            NextRhoIs0[i]=RhoIs0[i]+( sys0[m] - sys0[n])*(KPoints[i].sinkr[n] - KPoints[i].sinkr[m] );
            NextRhoRs1[i]=RhoRs1[i]+( sys1[m] - sys1[n])*(KPoints[i].coskr[n] - KPoints[i].coskr[m] );
            NextRhoIs1[i]=RhoIs1[i]+( sys1[m] - sys1[n])*(KPoints[i].sinkr[n] - KPoints[i].sinkr[m] );
            
            Eold += RhoIs0[i]*RhoIs0[i] + RhoRs0[i]*RhoRs0[i] + RhoIs1[i]*RhoIs1[i] + RhoRs1[i]*RhoRs1[i];
            Enew += NextRhoIs0[i]*NextRhoIs0[i] + NextRhoRs0[i]*NextRhoRs0[i] + NextRhoIs1[i]*NextRhoIs1[i] + NextRhoRs1[i]*NextRhoRs1[i];
		}

        double dE=(Enew-Eold)/sys.size();
        
		PreFactor=1.0;

		return dE;
	}

	void Accept(std::vector<signed char> & sys, std::vector<signed char> & sys0, std::vector<signed char> & sys1, const std::vector<StealthyChecker> & KPoints)
	{
		std::swap(sys[m], sys[n]);
		std::swap(sys0[m], sys0[n]);
		std::swap(sys1[m], sys1[n]);
		std::swap(RhoIs0, NextRhoIs0);
		std::swap(RhoRs0, NextRhoRs0);
		std::swap(RhoIs1, NextRhoIs1);
		std::swap(RhoRs1, NextRhoRs1);
	}

	SpinExchangeMove * clone() 
	{
		return new SpinExchangeMove(*this);
	}
};


#endif