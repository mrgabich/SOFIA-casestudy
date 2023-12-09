#include "mpbenchmark.h"
//FIM
#include "FIM.h"


int main(int argc,char *argv[])
{
	 FIM_Instantiate();

	//NUM_THREADS=omp_get_num_procs();
	omp_set_num_threads(NUM_THREADS);
 
    BenchmarkStartTime = omp_get_wtime();

	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		// local variables for calculating
		double altd, u0d;
		double throtl;
		double * trat = (double *)calloc(20,sizeof(double));
		double * tt = (double *)calloc(20,sizeof(double));
		double * prat = (double *)calloc(20,sizeof(double));
		double * pt = (double *)calloc(20,sizeof(double));
		double * eta = (double *)calloc(20,sizeof(double));
		double * gam = (double *)calloc(20,sizeof(double));
		double * cp = (double *)calloc(20,sizeof(double));

		double weight;
		double Rgas, alt, ts0=0, ps0=0, a0, u0, fsmach, q0, tsout, psout, rho0, cpair;
		double a8, a8d, a4, m2, mfr, isp;
		double epr, etr, npr, snpr;
		double fnet, fgros, dram, sfc, fa, eair, uexit, ues;
		double fnlb, fglb, drlb, flflo;
		double pexit, pfexit;
		double a8max, a8rat, a4p;
		double lcomp, lburn, lturb, lnoz, ncomp, nturb;
		double sblade, hblade, tblade, xcomp, ncompd;
		// variables for calculating time related, e.g.deadline etc.
		/* for each thread */
		double TimePoint = 0;
		double StartTime = 0, EndTime = 0, ExecTime = 0;
		double used = 0, ExecTotTime = 0, usedTime = 0;
		double StartPiTime, EndPiTime, PiTime;
		// variables for pi calculation
		const long num_steps = 1000000;
		double step = 1.0 / (double) num_steps;
		int i = 0;
		double x, pi, sum;
		// variables for input data
		double a, b, c, d;
		/* Initialize parameters */
		prat[4] = 1.0;
		eta[13] = 1.0;
		eta[4] = 1.0;
		eta[7] = 1.0;
		eta[3] = 1.0;
		eta[5] = 1.0;
		a4 = 0.418;
		a8max = 0.4;
		a8rat = .35;
		weight = 1000.;

		while (1) {
				
				if (NumPoints < LineCount) {
					int CurrentPoint=0;
					#pragma omp critical
					{
						NumPoints++;
						CurrentPoint = NumPoints;
					}
					if(CurrentPoint>LineCount){
						#pragma omp critical
						{
							NumPoints--;
						}
						NumPoints++;
						break;
					}
					/** Pi calculation */
					sum = 0;
					StartPiTime = omp_get_wtime();
					for (i = 0; i < num_steps; i++) {
						x = (i + 0.5) * step;
						sum += 4.0 / (1.0 + x * x);
					}
					{
						pi = sum * step;
					}
					EndPiTime = omp_get_wtime();
					PiTime = EndPiTime - StartPiTime;
					PiTime = PiTime;
					
					/** read input data ---Speed Altitude and Throttle */
					int index = CurrentPoint - 1;
					a = inputArray[index][0];
					b = inputArray[index][1];
					c = inputArray[index][2];
					d = inputArray[index][3];
					if (a < 0 || a > 1500) {
						printf("Warning : incorrect speed for point %d\n", CurrentPoint);
						u0d = 0;
					} else
						// Input speed in mph
						u0d = a;

					if (b < 0 || b > 50000) {
						printf("Warning : incorrect altitude for point %d\n", CurrentPoint);
						altd = 0;
					} else
						// Input altitude in feet
						altd = b;

					if (c < 45 || c > 90) {
						printf("Warning : incorrect throttle for point %d\n", CurrentPoint);
						throtl = 100;
					} else
						// Converting input throttle in %
						throtl = deg2rad(c, pi) * 100 * 2 / pi;

					if (d < 0) {
						printf("Warning : incorrect deadline for point %d\n", CurrentPoint);
						TimePoint = 0;
					} else {
						// Input time point
						TimePoint = d;
					}
					#pragma omp critical
					{
						TotalTimePoint += TimePoint;
					}
					/** ******** START CALCULATIONS **********/
					StartTime = omp_get_wtime();
					/*deduceInputs--------------------------------------------------------------------------*/
					Rgas = 1718.; /* ft2/sec2 R */
					alt = altd;
					if (alt < 36152.) {
						ts0 = 518.6 - 3.56 * alt / 1000.;
						ps0 = 2116. * fpow(ts0 / 518.6, 5.256);
					}
					if (alt >= 36152. && alt <= 82345.) { // Stratosphere
						ts0 = 389.98;
						ps0 = 2116. * .2236 * expo((36000. - alt) / (53.35 * 389.98));
					}
					if (alt >= 82345.) {
						ts0 = 389.98 + 1.645 * (alt - 82345) / 1000.;
						ps0 = 2116. * .02456 * fpow(ts0 / 389.98, -11.388);
					}

					a0 = sqroot(gama * Rgas * ts0); // speed of sound ft/sec

					u0 = u0d * 5280. / 3600.; // airspeed ft/sec
					fsmach = u0 / a0;
					q0 = gama / 2.0 * fsmach * fsmach * ps0;

					if (u0 > .0001)
						rho0 = q0 / (u0 * u0);
					else
						rho0 = 1.0;

					tt[0] = ts0 * (1.0 + .5 * (gama - 1.0) * fsmach * fsmach);
					pt[0] = ps0 * fpow(tt[0] / ts0, gama / (gama - 1.0));
					ps0 = ps0 / 144.;
					pt[0] = pt[0] / 144.;
					cpair = getCp(tt[0]); // BTU/lbm R
					tsout = ts0 - 459.6;
					psout = ps0;
					/*getThermo--------------------------------------------------------------------------*/
					double m5;
					double delhc, delhht, delhf, delhlt;
					double deltc, deltht, deltf, deltlt;

					// inlet recovery
					if (fsmach > 1.0) // supersonic
					{
						prat[2] = 1.0 - .075 * fpow(fsmach - 1.0, 1.35);
					} else {
						prat[2] = 1.0;
					}
					eta[2] = prat[2];

					// protection for overwriting input
					if (eta[3] < .5)
						eta[3] = .5;
					if (eta[5] < .5)
						eta[5] = .5;
					trat[7] = 1.0;
					prat[7] = 1.0;
					tt[2] = tt[1] = tt[0];
					pt[1] = pt[0];
					gam[2] = getGama(tt[2]);
					cp[2] = getCp(tt[2]);
					pt[2] = pt[1] * prat[2];

					// design - p3p2 specified - tt4 specified
					if (engine <= 2) // turbojet
					{
						prat[3] = p3p2d; // core compressor
						if (prat[3] < .5)
							prat[3] = .5;

						delhc = (cp[2] * tt[2] / eta[3]) * (fpow(prat[3], (gam[2] - 1.0) / gam[2]) - 1.0); // 0.25
						deltc = delhc / cp[2];
						pt[3] = pt[2] * prat[3];
						tt[3] = tt[2] + deltc;
						trat[3] = tt[3] / tt[2];
						gam[3] = getGama(tt[3]);
						cp[3] = getCp(tt[3]);
						tt[4] = tt4 * throtl / 100.0;
						gam[4] = getGama(tt[4]);
						cp[4] = getCp(tt[4]);
						trat[4] = tt[4] / tt[3];
						pt[4] = pt[3] * prat[4];
						delhht = delhc;
						deltht = delhht / cp[4];
						tt[5] = tt[4] - deltht;
						gam[5] = getGama(tt[5]);
						cp[5] = getCp(tt[5]);
						trat[5] = tt[5] / tt[4];
						prat[5] = fpow((1 - delhht / cp[4] / tt[4] / eta[5]), (gam[4] / (gam[4] - 1.0)));
						pt[5] = pt[4] * prat[5];

						// fan conditions
						prat[13] = 1.0;
						trat[13] = 1.0;
						tt[13] = tt[2];
						pt[13] = pt[2];
						gam[13] = gam[2];
						cp[13] = cp[2];
						prat[15] = 1.0;
						pt[15] = pt[5];
						trat[15] = 1.0;
						tt[15] = tt[5];
						gam[15] = gam[5];
						cp[15] = cp[5];
					}

					if (engine == 3) // turbofan
					{
						prat[13] = p3fp2d;
						if (prat[13] < .5)
							prat[13] = .5;

						delhf = (cp[2] * tt[2] / eta[13]) * (fpow(prat[13], (gam[2] - 1.0) / gam[2]) - 1.0);
						deltf = delhf / cp[2];
						tt[13] = tt[2] + deltf;
						pt[13] = pt[2] * prat[13];
						trat[13] = tt[13] / tt[2];
						gam[13] = getGama(tt[13]);
						cp[13] = getCp(tt[13]);
						prat[3] = p3p2d; // core compressor
						if (prat[3] < .5)
							prat[3] = .5;

						delhc = (cp[13] * tt[13] / eta[3]) * (fpow(prat[3], (gam[13] - 1.0) / gam[13]) - 1.0);
						deltc = delhc / cp[13];
						tt[3] = tt[13] + deltc;
						pt[3] = pt[13] * prat[3];
						trat[3] = tt[3] / tt[13];
						gam[3] = getGama(tt[3]);
						cp[3] = getCp(tt[3]);
						tt[4] = tt4 * throtl / 100.0;
						pt[4] = pt[3] * prat[4];
						gam[4] = getGama(tt[4]);
						cp[4] = getCp(tt[4]);
						trat[4] = tt[4] / tt[3];
						delhht = delhc;
						deltht = delhht / cp[4];
						tt[5] = tt[4] - deltht;
						gam[5] = getGama(tt[5]);
						cp[5] = getCp(tt[5]);
						trat[5] = tt[5] / tt[4];
						prat[5] = fpow((1.0 - delhht / cp[4] / tt[4] / eta[5]), (gam[4] / (gam[4] - 1.0)));
						pt[5] = pt[4] * prat[5];
						delhlt = (1.0 + byprat) * delhf;
						deltlt = delhlt / cp[5];
						tt[15] = tt[5] - deltlt;
						gam[15] = getGama(tt[15]);
						cp[15] = getCp(tt[15]);
						trat[15] = tt[15] / tt[5];
						prat[15] = fpow((1.0 - delhlt / cp[5] / tt[5] / eta[5]), (gam[5] / (gam[5] - 1.0)));
						pt[15] = pt[5] * prat[15];
					}
					tt[7] = tt7;
					prat[6] = 1.0;
					pt[6] = pt[15];
					trat[6] = 1.0;
					tt[6] = tt[15];
					gam[6] = getGama(tt[6]);
					cp[6] = getCp(tt[6]);
					tt[7] = tt[6] * trat[7];
					pt[7] = pt[6] * prat[7];
					gam[7] = getGama(tt[7]);
					cp[7] = getCp(tt[7]);

					// engine press ratio EPReair
					epr = prat[7] * prat[15] * prat[5] * prat[4] * prat[3] * prat[13];
					// engine temp ratio ETR
					etr = trat[7] * trat[15] * trat[5] * trat[4] * trat[3] * trat[13];
					/*getGeo--------------------------------------------------------------------------*/
					// limits compressor face
					a8max = .75 * sqroot(etr) / epr;

					// mach number to < .5
					if (a8max > 1.0)
						a8max = 1.0;

					if (a8rat > a8max)
						a8rat = a8max;

					// dumb down limit - a8 schedule
					a8rat = a8max;

					a8 = a8rat * acore;
					a8d = a8 * prat[7] / sqroot(trat[7]);

					// assumes choked a8 and a4
					a4 = a8 * prat[5] * prat[15] * prat[7] / sqroot(trat[7] * trat[5] * trat[15]);
					a4p = a8 * prat[15] * prat[7] / sqroot(trat[7] * trat[15]);

					// size parameters for weight
					ncomp = (int) (1.0 + p3p2d / 1.5);
					if (ncomp > 15)
						ncomp = 15;

					sblade = .02;
					hblade = sqroot(2.0 / 3.1415926);
					tblade = .2 * hblade;

					xcomp = ncomp * (tblade + sblade);
					ncompd = ncomp;
					if (engine == 3) { // fan geometry
						ncompd = ncomp + 3;
						xcomp = ncompd * (tblade + sblade);
					}

					lcomp = xcomp;
					lburn = hblade;

					nturb = 1 + ncomp / 4;
					if (engine == 3)
						nturb = nturb + 1;

					lturb = nturb * (tblade + sblade);
					lnoz = lburn;
					if (engine == 2)
						lnoz = 3.0 * lburn;
					/*calcPerf--------------------------------------------------------------------------*/
					double fac1, game, cpe, cp3, rg, rg1;

					rg1 = 53.3;
					rg = cpair * (gama - 1.0) / gama;
					cp3 = getCp(tt[3]); // BTU/lbm R
					ues = 0.0;
					game = getGama(tt[5]);
					fac1 = (game - 1.0) / game;
					cpe = getCp(tt[5]);
					if (eta[7] < .8)
						eta[7] = .8; // protection during overwriting
					if (eta[4] < .8)
						eta[4] = .8;
					pt[8] = epr * prat[2] * pt[0];
					eair = getAir(1.0, game) * 144. * a8 * pt[8] / 14.7 / sqroot(etr * tt[0] / 518.);
					m2 = getMach(0, eair * sqroot(tt[0] / 518.) / (prat[2] * pt[0] / 14.7 * acore * 144.), gama);
					npr = pt[8] / ps0;
					uexit = sqroot(2.0 * Rgas / fac1 * etr * tt[0] * eta[7] * (1.0 - fpow(1.0 / npr, fac1)));

					if (npr <= 1.893)
						pexit = ps0;
					else
						pexit = .52828 * pt[8];

					fgros = (uexit + (pexit - ps0) * 144. * a8 / eair) / g0;

					// turbo fan -- added terms for fan flow
					if (engine == 3) {
						fac1 = (gama - 1.0) / gama;
						snpr = pt[13] / ps0;
						ues = sqroot(2.0 * Rgas / fac1 * tt[13] * eta[7] * (1.0 - fpow(1.0 / snpr, fac1)));
						m2 = getMach(0, eair * (1.0 + byprat) * sqroot(tt[0] / 518.)
								/ (prat[2] * pt[0] / 14.7 * afan * 144.), gama);

						if (snpr <= 1.893)
							pfexit = ps0;
						else
							pfexit = .52828 * pt[13];
						fgros = fgros + (byprat * ues + (pfexit - ps0) * 144. * byprat * acore / eair) / g0;
					}

					// ram drag
					dram = u0 / g0;
					if (engine == 3)
						dram = dram + u0 * byprat / g0;

					// mass flow ratio
					if (fsmach > .01)
						mfr = getAir(m2, gama) * prat[2] / getAir(fsmach, gama);
					else
						mfr = 5.;

					// net thrust
					fnet = fgros - dram;

					// thrusts in pounds
					fnlb = fnet * eair;
					fglb = fgros * eair;
					drlb = dram * eair;

					// fuel-air ratio and sfc
					fa = (trat[4] - 1.0) / (eta[4] * fhv / (cp3 * tt[3]) - trat[4]) + (trat[7] - 1.0)
							/ (fhv / (cpe * tt[15]) - trat[7]);
					if (fnet > 0.0) {
						sfc = 3600. * fa / fnet;
						if (sfc < 0)
							sfc = 0.0;
						flflo = sfc * fnlb;
						isp = (fnlb / flflo) * 3600.;
					} else {
						fnlb = 0.0;
						flflo = 0.0;
						sfc = 0.0;
						isp = 0.0;
					}

					// weight calculation
					if (engine == 1) {
						weight = .12754 * sqroot(acore * acore * acore)
								* (dcomp * lcomp + dburner * lburn + dturbin * lturb + dnozl * lnoz);
					}
					if (engine == 2) {
						weight = .08533 * sqroot(acore * acore * acore)
								* (dcomp * lcomp + dburner * lburn + dturbin * lturb + dnozl * lnoz);
					}
					if (engine == 3) {
						weight = .08955
								* acore
								* ((1.0 + byprat) * dfan * 4.0 + dcomp * (ncomp - 3) + dburner + dturbin * nturb + dburner * 2.0)
								* sqroot(acore / 6.965);
					}
					/*--------------------------------------------------------------------------*/
					EndTime = omp_get_wtime();
					ExecTime = (EndTime - StartTime);
					used = (ExecTime + PiTime) / TimePoint;
					ExecTotTime = ExecTime + PiTime;
					usedTime = (ExecTime + PiTime) - TimePoint;
					#pragma omp critical
					{
						TotalTime += ExecTotTime;
						TotalUsed += used;
					}

					/*********** PRINT RESULTS ************/
					if (used > 1) {
						#pragma omp critical
						{
							NumMissed++;
							TotalTimePoint += usedTime;
						}
					}
					outputArray[index][0] = id;
					outputArray[index][1] = ExecTotTime;
					outputArray[index][2] = u0d;
					outputArray[index][3] = altd;
					outputArray[index][4] = throtl;
					outputArray[index][5] = fsmach;
					outputArray[index][6] = psout;
					outputArray[index][7] = tsout;
					outputArray[index][8] = fnlb;
					outputArray[index][9] = fglb;
					outputArray[index][10] = drlb;
					outputArray[index][11] = flflo;
					outputArray[index][12] = sfc;
					outputArray[index][13] = eair;
					outputArray[index][14] = weight;
					outputArray[index][15] = fnlb / weight;
					outputArray[index][16] = used * 100;
					outputArray[index][17] = CurrentPoint;

				}//end of if
				else{	break;	}
		}
		//end of while
		free(trat);
		free(tt);
		free(prat);
		free(pt);
		free(eta);
		free(gam);
		free(cp);

	}//end of parallel
	BenchmarEndTime = omp_get_wtime();
	//printResult();

	//FIM
    FIM_exit();
	return 0;
}

void printResult(){
	// header for results
	
	printf("Spd| Alt |  Thr| Mach|Press| Temp| Fnet|Fgros|RamDr|FlFlo|TSFC|Airfl|Weight|Fn/W\n");
	int i=0;
	for (i = 0; i < LineCount; i++) {
		printf("%4.0f|%5.0f|%5.1f|%5.3f|%5.2f|%5.1f|%5.0f|%5.0f|%5.0f|%5.0f|%4.2f|%5.1f|%6.2f|%4.2f\n",
								outputArray[i][2], outputArray[i][3],
								outputArray[i][4], outputArray[i][5], outputArray[i][6], outputArray[i][7],
								outputArray[i][8], outputArray[i][9], outputArray[i][10], outputArray[i][11],
								outputArray[i][12], outputArray[i][13], outputArray[i][14], outputArray[i][15]);
	}
	
	printf("%d\n", NumMissed);
	//printf("Thread response time sum:%f\n", TotalTime);
	printf("Number of threads : %d\n", NUM_THREADS);
	printf("Number of points : %d\n", NumPoints);
	double benchmarkTotalTime = (BenchmarEndTime - BenchmarkStartTime);
	printf("%f\n", benchmarkTotalTime);

}


/* Utility to convert degree in radian */
double deg2rad(double deg,double pi)
{
	return(deg/180*pi);
}

/* Utility to get gamma as a function of temperature */
double getGama(double temp) {
	double number, a, b, c, d;
	a = -7.6942651e-13;
	b = 1.3764661e-08;
	c = -7.8185709e-05;
	d = 1.436914;
	number = a * temp * temp * temp + b * temp * temp + c * temp + d;
	return (number);
}

/* Utility to get the Mach number given the corrected airflow per area */
double getMach(int sub, double corair, double gama1) {
	double number, chokair; // iterate for mach number
	double deriv, machn, macho, airo, airn;
	int iter;
	chokair = getAir(1.0, gama1);
	if (corair > chokair) {
		number = 1.0;
		return (number);
	} else {
		airo = .25618; // initial guess
		if (sub == 1)
			macho = 1.0; // sonic
		else {
			if (sub == 2)
				macho = 1.703; // supersonic
			else
				macho = .5; // subsonic
			iter = 1;
			machn = macho - .2;
			while (fabs(corair - airo) > .0001 && iter < 20) {
				airn = getAir(machn, gama1);
				deriv = (airn - airo) / (machn - macho);
				airo = airn;
				macho = machn;
				machn = macho + (corair - airo) / deriv;
				++iter;
			}
		}
		number = macho;
	}
	return (number);
}

/* Utility to get the corrected airflow per area given the Mach number */
double getAir(double mach, double gama2) {
	double number, fac1, fac2;
	fac2 = (gama2 + 1.0) / (2.0 * (gama2 - 1.0));
	fac1 = fpow((1.0 + .5 * (gama2 - 1.0) * mach * mach), fac2);
	number = .50161 * sqroot(gama2) * mach / fac1;

	return (number);
}

/* Analysis for Rayleigh flow */
double getRayleighLoss(double mach1, double ttrat, double tlow) {
	double number;
	double wc1, wc2, mgueso, mach2, g1, gm1, g2, gm2;
	double fac1, fac2, fac3, fac4;
	g1 = getGama(tlow);
	gm1 = g1 - 1.0;
	wc1 = getAir(mach1, g1);
	g2 = getGama(tlow * ttrat);
	gm2 = g2 - 1.0;
	number = .95;
	// iterate for mach downstream
	mgueso = .4; // initial guess
	mach2 = .5;
	while (fabs(mach2 - mgueso) > .0001) {
		mgueso = mach2;
		fac1 = 1.0 + g1 * mach1 * mach1;
		fac2 = 1.0 + g2 * mach2 * mach2;
		fac3 = fpow((1.0 + .5 * gm1 * mach1 * mach1), (g1 / gm1));
		fac4 = fpow((1.0 + .5 * gm2 * mach2 * mach2), (g2 / gm2));
		number = fac1 * fac4 / fac2 / fac3;
		wc2 = wc1 * sqroot(ttrat) / number;
		mach2 = getMach(0, wc2, g2);
	}
	return (number);
}

/* Utility to get cp as a function of temperature */
double getCp(double temp) {
	double number, a, b, c, d;
	// BTU/R
	a = -4.4702130e-13;
	b = -5.1286514e-10;
	c = 2.8323331e-05;
	d = 0.2245283;
	number = a * temp * temp * temp + b * temp * temp + c * temp + d;
	return (number);
}

double fpow(double x, double y) {
	int partieEntiere = (int) y;
	// If x<0 and y not integer
	if (x < 0 && (double) partieEntiere != y) {
		printf("error power undefined\n");
		return 0;
	}
	// If x<0 and y integer
	else if (x < 0)
		return power(x, partieEntiere);
	return power(x, partieEntiere) * expo((y - partieEntiere) * log(x));
}

double sqroot(double number) {
	double x0, x, prec = 1;
	if (number < 0) {
		printf("error sqroot\n");
		return (0);
	}

	x = (1 + number) / 2;
	while ((prec > 0.0001) || (prec < -0.0001)) {
		x0 = x;
		x = 0.5 * (x0 + number / x0);
		prec = (x - x0) / x0;
	}
	return (x);
}

double fabs(double x) {
	if (x < 0)
		return -x;
	else
		return x;
}

double log(double x) {
	double number = 0;
	double coeff = -1;
	int i = 1;
	if (x <= 0) {
		printf("error log undefined\n");
		return 0;
	}

	if (x == 1)
		return 0;

	if (x > 1)
		return -log(1 / x);

	// 0<x<1
	// log : x - x^2/2 + x^3/3 - x^4/4...
	while (fabs(coeff) > PRECISION) {
		coeff *= 1 - x;
		number += coeff / i;
		i++;
	}
	return number;
}

double power(double x, int y) {
	double number = 1;
	int i;
	// x^(-y) = 1/(x^y)
	if (y < 0)
		return 1 / (power(x, -y));

	for (i = 0; i < y; i++)
		number *= x;

	return number;
}

double expo(double x) {
	double number = 1;
	double coeff = 1;
	int i = 1;
	// if x > log(DBL_MAX)
	if (x > 709.782712893384)
		return expo(709.78); // Infinite value

	// exp : x^0/0! + x^1/1! + x^2/2! + x^3/3!
	while (fabs(coeff) > PRECISION) {
		coeff *= x / i;
		number += coeff;
		i++;
	}

	return number;
}
