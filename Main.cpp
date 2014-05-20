#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <set>


#include "MonteCarlo.h"
#include "StealthyChecker.h"


void OutputConfig(const std::vector<signed char> & sys, std::ostream & os)
{
	for(auto iter=sys.begin(); iter!=sys.end(); iter++)
	{
		if(*iter==2)
			os<<'+';
		else if(*iter==1)
			os<<'o';
        else
            os<<'-';
	}
    std::cout<<"in here"<<'\n';
}

std::vector< std::vector< signed char > > GetConf(double conf_option)
{
    std::vector<signed char> conf, conf0, conf1;
    std::vector< std::vector< signed char > > allconf;
    if (conf_option == 1)
    {
        std::cout<<"Input configuration (string of +,o,-): ";
        std::string conf_in;
        std::getline(std::cin.ignore(), conf_in);
        int counter0 = 0, counter1 = 0;
        for (size_t i = 0; i < conf_in.size(); i++)
        {
            signed char current_char = conf_in[i];
            if (current_char == '+')
            {
                conf.push_back(1);
                conf0.push_back(1);
                conf1.push_back(0);
                counter0++;
            }
            else if (current_char == 'o')
            {
                conf.push_back(2);
                conf0.push_back(0);
                conf1.push_back(1);
                counter1++;
            }
            else if (current_char == '-')
            {
                conf.push_back(0);
                conf0.push_back(0);
                conf1.push_back(0);
            }
            else
                std::cout<<"Invalid character (should be +,o,- only)";
            //std::cout<<current_char<<'\t'<<(int)conf[i]<<'\n';
        }
        std::cout<<"Number particles "<<conf_in.size()<<'\n';
        std::cout<<"Number species0 (+) "<<counter0<<'\n';
        std::cout<<"Number species1 (o) "<<counter1<<'\n';
        std::cout<<"Number species2 (-) "<<conf_in.size() - counter0 - counter1<<'\n';
    }
    else if (conf_option == 0)
    {
        std::cout<<"Input sizes: ";
        std::vector<int> size(2, 0);
        std::cin>>size[0];
        std::cin>>size[1];
        std::cout<<"Input number of particles (for first two species, of three): ";
        size_t nbr0;
        size_t nbr1;
        std::cin>>nbr0;
        std::cin>>nbr1;
        size_t NbrSpin=size[0]*size[1];
        assert(nbr0 + nbr1<=NbrSpin);
        for (size_t i=0; i<nbr0; i++)
        {
            conf.push_back(1);
            conf0.push_back(1);
            conf1.push_back(0);
        }
        for (size_t i=nbr0; i<nbr0+nbr1; i++)
        {
            conf.push_back(2);
            conf0.push_back(0);
            conf1.push_back(1);
        }
        for (size_t i=nbr0+nbr1; i<NbrSpin; i++)
        {
            conf.push_back(0);
            conf0.push_back(0);
            conf1.push_back(0);
        }
    }
    else
        std::cout<<"Illegal option, choose 0 or 1";

    allconf.push_back(conf);
    allconf.push_back(conf0);
    allconf.push_back(conf1);

    return allconf;
}

int SpinMC(std::vector< std::vector< signed char > > allconf)
{
    std::vector<signed char> conf, conf0, conf1;
    conf = allconf[0];
    conf0 = allconf[1];
    conf1 = allconf[2];
    
    std::vector<int> size(2, 0);

    size_t NbrSpin = (size_t)conf.size();
    int sidelength = (int)sqrt(NbrSpin);
    size[0] = sidelength;
    size[1] = sidelength;
    
	std::vector<StealthyChecker> KPoints;
	std::vector<int> KConstraint(2, 0);
	//size_t NbrKPoints;
	//std::cout<<"Input number of k points:";
	//std::cin>>NbrKPoints;
	//for(size_t i=0; i<NbrKPoints; i++)
	//{
	//	std::cout<<"Input k"<<i<<':';
	//	std::cin>>KConstraint[0];
	//	std::cin>>KConstraint[1];
	//	KPoints.push_back(StealthyChecker(size, KConstraint));
	//}
	std::set<std::vector<int> > ks;
	for(;;)
	{
		std::cout<<"a to add k point, r to add all k points in a radius, f to finish :";
		char command;
		std::cin>>command;
		if(command=='a')
		{
			std::cout<<"Input k"<<':';
			std::cin>>KConstraint[0];
			std::cin>>KConstraint[1];
			ks.insert(KConstraint);
			//KPoints.push_back(StealthyChecker(size, KConstraint));
		}
		else if(command=='r')
		{
			std::cout<<"Input radius:";
			double radius;
			std::cin>>radius;
			for(size_t i=0; i<radius; i++)
			{
				for(size_t j=0; j<radius; j++)
				{
					if( j*j+i*i>radius*radius)
						break;
					if(i==0 && j==0)
						continue;
					KConstraint[0]=i;
					KConstraint[1]=j;
					ks.insert(KConstraint);
					if(i!=0 && j!=0)
					{
						KConstraint[0]=i;
						KConstraint[1]=(-1)*j;
						ks.insert(KConstraint);
					}
				}
			}
		}
		else if(command=='f')
			break;
		else
			std::cout<<"unrecognized command!\n";
	}
	std::cout<<"constraints are:\n";
	for(std::set<std::vector<int> >::iterator iter=ks.begin(); iter!=ks.end(); iter++)
	{
		KPoints.push_back(StealthyChecker(size, *iter));
		std::cout<<iter->at(0)<<" \t"<<iter->at(1)<<'\n';
	}

	std::cout<<"Input random seed:";
	int seed;
	std::cin>>seed;
	MonteCarlo< std::vector<signed char>, const std::vector<StealthyChecker> & , SpinExchangeMove> mc(conf, conf0, conf1, seed, Energy(conf0, conf1, KPoints, NbrSpin));
	SpinExchangeMove move(conf, conf0, conf1, KPoints);
	//ThermoCool c1(0.9999997, 100.0, 1e-4, 100000);
	//ExpCool c1(0.999999, 100.0, 1e-7, 100000);
	//ZeroCool c1(100000);
	CoolingSchedule * pc1=nullptr;

	while(pc1==nullptr)
	{
		std::cout<<"Choose cooling schedule (z : keep T=0, t : Thermodynamic, e : Exponential)";
		char c;
		std::cin>>c;
		if(c=='z')
		{
			std::cout<<"Input Number of sweeps";
			size_t NbrSweeps;
			std::cin>>NbrSweeps;
			pc1=new ZeroCool(NbrSweeps*size[0]*size[1]);
		}
		else if(c=='t')
		{
			std::cout<<"Input Cooling speed, intial temperature, stopping temperature, and T=0 number of sweeps";
			double CoolingSpeed;
			std::cin>>CoolingSpeed;
			double Tinit;
			std::cin>>Tinit;
			double Tcut;
			std::cin>>Tcut;
			size_t NbrSweeps;
			std::cin>>NbrSweeps;
			double alpha = std::pow(0.9998, CoolingSpeed/size[0]/size[1]);
			pc1=new ThermoCool(alpha, Tinit, Tcut, NbrSweeps*size[0]*size[1]);
		}
		else if(c=='e')
		{
			std::cout<<"Input Cooling speed, intial temperature, stopping temperature, and T=0 number of sweeps";
			double CoolingSpeed;
			std::cin>>CoolingSpeed;
			double Tinit;
			std::cin>>Tinit;
			double Tcut;
			std::cin>>Tcut;
			size_t NbrSweeps;
			std::cin>>NbrSweeps;
			double alpha = std::pow(0.9994, CoolingSpeed/size[0]/size[1]);
			pc1=new ExpCool(alpha, Tinit, Tcut, NbrSweeps*size[0]*size[1]);
		}
		else
			std::cout<<"Illegal cooling schedule!\n";
	}

	::ProgramStart = std::time(nullptr);
	double TPrev=0;
	size_t * pNbrTrial = & mc.MoveCount;
	size_t * pNbrAcceptance = & mc.AcceptanceCount;
    
    double counter = 0;
	mc.Anneal(KPoints, *pc1, move, [&](double & E, double Temperature, std::vector<signed char> & sys, std::vector<signed char> & sys0, std::vector<signed char> & sys1)
	{
        counter++;
        double newE=Energy(conf0, conf1, KPoints, NbrSpin);
        if(std::abs((newE-E))>1e-10)
        {
            std::cerr<<"Error : energy not consistent, newE="<<newE<<", oldE="<<E<<'\n';
        }

        if(newE < 1e-15)
        {
            std::cout<<"FOUND! Energy "<<newE<<", conf ";
            OutputConfig(conf, std::cout);
            std::cout<<"\n";
        }
        if(Temperature!=TPrev)
            std::cout<<"Time="<<std::time(nullptr)-ProgramStart<<"\t Trials="<<*pNbrTrial<<"\t Acceptances="<<*pNbrAcceptance<<" \tT="<<Temperature<<" \tE="<<E<<'\n';
        TPrev=Temperature;
        E=newE;
	} );


	std::cout<<"E="<<Energy(conf0, conf1, KPoints, NbrSpin)<<", config is:\n";
	OutputConfig(conf, std::cout);
	std::cout<<'\n';

	delete pc1;

	return 0;
}

#include <ctime>
int main()
{
    std::cout<<"Use existing (1) or new (0)";
    double conf_option;
    std::cin>>conf_option;
    std::vector< std::vector< signed char > > allconf;
    allconf = GetConf(conf_option);
    return SpinMC(allconf);
}