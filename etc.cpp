#include "etc.h"

std::fstream logfile("DetailedLog.txt", std::fstream::app|std::fstream::out);

unsigned long MCThreadsPerConfiguration=0;
unsigned long MCNbrParallelConfigurations=0;
unsigned long PTParallelConfigurationsPerSystem=8;
unsigned long OptimizationNbrThreadsPerTrial=0;

bool AlsoWriteEPS=false;

double TwoBodyDistance_MaxLength=4;

double PlotPhononFrequencies_MaxFrequencySquared_Override=0.0;
size_t Verbosity = 1;
time_t ProgramStart;