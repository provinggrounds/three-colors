#ifndef ETC_INCLUDED
#define ETC_INCLUDED

#include <cmath>

const double LengthPrecision=1e-4;
const double EnergyPrecision=1e-4;
const double MaxEnergy=1e10;
const double pi=3.1415926535897932384626433832795028841971693993751058209;
typedef unsigned short DimensionType;
const DimensionType MaxDimension = 4;
const bool UseCorrectedPotential = false;

extern bool AlsoWriteEPS;//when write bmp files, also write eps

extern unsigned long MCThreadsPerConfiguration;
extern unsigned long MCNbrParallelConfigurations;
extern unsigned long PTParallelConfigurationsPerSystem;
extern unsigned long OptimizationNbrThreadsPerTrial;

extern double TwoBodyDistance_MaxLength;

extern double PlotPhononFrequencies_MaxFrequencySquared_Override;

#include <fstream>
extern std::fstream logfile;

extern size_t Verbosity;

struct Empty
{
	Empty()
	{}
};

#include <ctime>
extern time_t ProgramStart;

#endif
