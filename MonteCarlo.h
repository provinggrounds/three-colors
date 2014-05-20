#ifndef MONTECARLO_INCLUDED
#define MONTECARLO_INCLUDED

//General-Purpose Monte Carlo, not necessary for particles

#include "etc.h"
#include "RandomGenerator.h"
#include "Move.h"
#include <functional>
#include <vector>

class CoolingSchedule
{
public:
	bool Continue;
	double Temperature;
	size_t NbrTry;

	virtual void Report(double Energy) =0;
	//when initialized, the variables are set to the 1st stage cooling.
	//after class SimulatedAnnealing performs these MC trials, it will call Report to report it's current energy
	//the class need to decide if another stage is needed, and update the variables
};

//General-Purpose Monte Carlo, not necessary for particles
template<typename System, typename AdditionalInfo, typename MCMove> class MonteCarlo
{
protected:
	RandomGenerator gen;
public:

	double RecordEnergy;//this class keeps a log of the system energy, this is valid only if the potential is never changed
	size_t MoveCount;
	size_t AcceptanceCount;
	System * pSys;
	System * pSys0;
	System * pSys1;
    
	//Some trial moves, like particle displacement, has step size that needs to be adjusted.
	//The adjustment can be automatic. However, it must be turned off during sampling.
	//Set LockStepSize=true to turn off automatic adjustment of trial move parameters.
	bool LockStepSize;

	MonteCarlo(System & sys, System & sys0, System & sys1, int RandomSeed, double Energy) : gen(RandomSeed)
	{
		this->MoveCount=0;
		this->AcceptanceCount=0;
		this->pSys= & sys;
		this->pSys0= & sys0;
		this->pSys1= & sys1;
		this->RecordEnergy=Energy;
		this->LockStepSize=false;
	}
	virtual ~MonteCarlo()
	{
	}

	virtual void Move(double Temperature, size_t Repeat, MCMove & move, AdditionalInfo add)
	{
		for(size_t i=0; i<Repeat; i++)
		{
			MoveCount++;
			double PreFactor=1.0;
			double dE=move.DeltaEnergy(*this->pSys, *this->pSys0, *this->pSys1, add, this->gen, PreFactor, LockStepSize);
			if(Temperature ==0)
			{
				if(dE>0)
					continue;
			}
			else if(gen.RandomDouble() > PreFactor*std::exp((-1)*dE/Temperature))
				continue;

			move.Accept(*this->pSys, *this->pSys0, *this->pSys1, add);
			this->AcceptanceCount++;
			this->RecordEnergy+=dE;
		}
	}
	virtual void Anneal(AdditionalInfo add, CoolingSchedule & cool, MCMove & move, std::function<void(double & Energy, double Temperature, System & sys, System & sys0, System & sys1)> CallBack)
	{
		while(cool.Continue)
		{
			this->Move(cool.Temperature, cool.NbrTry, move, add);
			CallBack(this->RecordEnergy, cool.Temperature, *pSys, *pSys0, *pSys1);

			cool.Report(this->RecordEnergy);
		}
	}
};


const size_t StageSize=1000;
class ExpCool : public CoolingSchedule
{
	double StageAlpha;
	double Tcut;
	signed long ZeroMove;
public:
	ExpCool(double Alpha, double Tinit, double Tcut, long ZeroMove) : Tcut(Tcut), ZeroMove(ZeroMove)
	{
		this->StageAlpha=std::pow(Alpha, static_cast<double>(StageSize));
		this->Continue=true;
		this->Temperature=Tinit;
		this->NbrTry=StageSize;
	}
	virtual void Report(double Energy)
	{
		//////////////////////////
		if(this->Temperature==0)
		{
			this->ZeroMove-=this->NbrTry;
			if(this->ZeroMove<=0)
				this->Continue=false;
		}
		else
		{
			this->Temperature*=this->StageAlpha;
			if(this->Temperature<this->Tcut)
			{
				this->Temperature=0;
				if(this->NbrTry>this->ZeroMove)
					this->NbrTry=this->ZeroMove;
			}
		}
	}
};



const size_t ReportTime=1000;
class ZeroCool : public CoolingSchedule
{
	size_t NbrReport;

public:
	ZeroCool(long ZeroMoveTime)
	{
		this->Temperature=0.0;
		this->NbrTry=ZeroMoveTime/ReportTime;
		this->Continue=true;
		this->NbrReport=0;
	}
	virtual void Report(double Energy)
	{
		this->NbrReport++;
		if(this->NbrReport>ReportTime)
			this->Continue=false;
	}
};


#include "AnalyzeData.h"
const size_t SampleInterval = 100;
const double VsCoeff=500;
const size_t InitAnalyzeSize = 1000;
const size_t MaxAnalyzeSize = 50000;
const size_t MinAnalyzeSize = 500;
class ThermoCool : public CoolingSchedule
{
	double Vs;
	double Tcut;
	signed long ZeroMove;//deprecated parameter

	size_t AnalyzeSize;
	size_t DataAllTheSameCount;

	std::vector<double> data;
public:
	ThermoCool(double Alpha, double Tinit, double Tcut, long ZeroMove) : Tcut(Tcut), ZeroMove(ZeroMove)
	{
		this->Vs=(1.0-Alpha)*SampleInterval*VsCoeff;
		this->Continue=true;
		this->Temperature=Tinit;
		this->NbrTry=SampleInterval;

		this->AnalyzeSize=InitAnalyzeSize;
		this->DataAllTheSameCount=0;
	}
	virtual void Report(double Energy)
	{
		//////////////////////////
		if(this->Temperature==0)
		{
			this->Continue=false;
		}
		else
		{
			data.push_back(Energy);

			if(data.size()>=AnalyzeSize)
			{
				double Average, error, Corrtime;
				AnalyzeData(data, Average, error, Corrtime);
				double H2c=error*error*data.size()/(1+2*Corrtime);
				double dlnT=this->Vs*this->Temperature/Corrtime/std::sqrt(H2c);

				//debug temp
				//std::fstream file("ErrorLog.txt", std::fstream::out);
				//for(auto iter=data.begin(); iter!=data.end(); iter++)
				//	file<<(*iter)<<'\n';
				//file.close();
				//exit(0);
				bool DataAllTheSame=true;
				for(std::vector<double>::iterator iter=data.begin()+1; iter!=data.end(); iter++)
				{
					if(data[0]!=(*iter))
					{
						DataAllTheSame=false;
						this->DataAllTheSameCount=0;
						break;
					}
				}
				if(DataAllTheSame)
				{
					this->DataAllTheSameCount++;
					//std::cout<<"DataAllTheSameCount="<<this->DataAllTheSameCount<<'\n';
					//energy is essentially constant, stop cooling
					if(this->DataAllTheSameCount>100)
					{
						this->Temperature=0;
						if(this->ZeroMove==0)
							this->Continue=false;
						else
							this->NbrTry=this->ZeroMove;
					}
				}
				else if( (!(dlnT>0)) )
				{
					if(this->AnalyzeSize < MaxAnalyzeSize)
					{
						this->Vs*=2;
						this->AnalyzeSize*=2;
					}
					this->Temperature/=std::exp(0.01);//cool very cautiously when can't analyze data
				}
				else if(dlnT>0.1)
				{
					//std::cout<<"Warning in ThermoCool : Temperature dropping too quickly, decrease sample size.\n";
					if(this->AnalyzeSize>MinAnalyzeSize)
					{
						this->Vs/=2;
						this->AnalyzeSize/=2;
					}
					this->Temperature/=std::exp(0.1);
				}
				else
				{
					this->Temperature/=std::exp(dlnT);
				}

				data.clear();
			}

			if(this->Temperature<this->Tcut)
			{
				this->Temperature=0;
				if(this->ZeroMove==0)
					this->Continue=false;
				else
					this->NbrTry=this->ZeroMove;
			}
		}
	}
};

#endif
