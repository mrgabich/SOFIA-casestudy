//#include<stdio.h>
//#include<stdlib.h>
#include<omp.h>

int main();
void InitializeArray();
void printResult();
double deg2rad(double deg,double pi);
double sqroot(double numb1er);
double fabs(double x);
double log(double x);
double expo(double x);
double fpow(double x, double y);
double power(double x, int y);
double getCp(double temp);
double getRayleighLoss(double mach1, double ttrat, double tlow);
double getAir(double mach, double gama2);
double getMach(int sub, double corair, double gama1);
double getGama(double temp);

int engine = 3;
double BenchmarkStartTime, BenchmarEndTime;
int NUM_THREADS = 2;
const double PRECISION = 0.0001;
// global constant variables
const double g0 = 32.2;
const double gama = 1.4;
const double tt4 = 2500.0;
const double tt7 = 2500.0;
const double p3p2d = 8.0;
const double p3fp2d = 2.0;
const double byprat = 1.0;
const double fhv = 18600.0;
const double acore = 2.0;
const double afan = 2.0;
const double dfan = 293.02;
const double dcomp = 293.02;
const double dburner = 515.2;
const double dturbin = 515.2;
const double dnozl = 515.2;
// arrays, store input and output data
const double inputArray[48][4]= {
            {0, 0, 90, 0.05},
			{100, 0, 90, 0.05},
			{200, 1000, 90, 0.05},
			{300, 2000, 90, 0.05},
			{300, 3000, 90, 0.05},
			{300, 4000, 90, 0.05},
			{300, 6000, 90, 0.05},
			{300, 7000, 90, 0.05},
			{300, 8000, 90, 0.05},
			{300, 9000, 90, 0.05},
			{350, 10000, 90, 0.05},
			{350, 15000, 90, 0.05},
			{400, 20000, 90, 0.05},
			{450, 25000, 90, 0.05},
			{500, 30000, 90, 0.05},
			{200, 1000, 90, 0.05},
			{300, 2000, 90, 0.05},
			{300, 3000, 90, 0.05},
			{300, 4000, 90, 0.05},
			{300, 5000, 90, 0.05},
			{300, 6000, 90, 0.05},
			{300, 7000, 90, 0.05},
			{300, 8000, 90, 0.05},
			{300, 9000, 90, 0.05},
			{350, 10000, 90, 0.05},
			{350, 15000, 90, 0.05},
			{400, 20000, 90, 0.05},
			{450, 25000, 90, 0.05},
			{500, 30000, 90, 0.05},
			{600, 30000, 90, 0.05},
			{600, 30000, 60, 0.05},
			{600, 25000, 50, 0.05},
			{600, 20000, 50, 0.05},
			{600, 15000, 50, 0.05},
			{500, 15000, 50, 0.05},
			{400, 15000, 50, 0.05},
			{400, 10000, 50, 0.05},
			{300, 10000, 50, 0.05},
			{285, 9000, 50, 0.05},
			{270, 8000, 50, 0.05},
			{255, 7000, 50, 0.05},
			{240, 6000, 50, 0.05},
			{225, 5000, 50, 0.05},
			{210, 4000, 50, 0.05},
			{195, 3000, 50, 0.05},
			{180, 2000, 50, 0.05},
			{165, 1000, 50, 0.05},
			{150, 0, 50, 0.05}};
double outputArray[48][18]={0};

int LineCount = 48;
int NumPoints = 0, NumMissed = 0;
double TotalTimePoint = 0;
double TotalTime = 0, TotalUsed = 0;