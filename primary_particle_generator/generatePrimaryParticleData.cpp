#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include "CLHEP/Random/Randomize.h"
#include "CLHEP/Units/PhysicalConstants.h"

/*
 * MeV kinetic energy of primary neutrino used if REACTOR_NEUTRINO_SPECTRUM_FLAG
 * = 0. Becareful because the threshold for inverse beta decay is not exactly
 * 1.8MeV but ~1.806MeV. If you set this value as 1.8, there will be no events
 * produced. Try using a minimum value of 1.81.
 */
#define PRIMARY_NEUTRINO_KINETIC_ENERGY 1.81

/*
 * Number of ITERATIONSs for data generation.
 */
#define ITERATIONS 10

/*
 * MeV kinetic energy of neutron coming off from inverse beta decay that is used
 * by program if FILTER_NEUTRON_KE is set to 1. Value is not used if
 * FILTER_NEUTRON_KE is set otherwise.
 */
#define NEUTRON_KINETIC_ENERGY .1

/*
 * Epsilon tolerence in MeV from value of NEUTRON_KINETIC_ENERGY.
 */
#define EPSILON .00001

/*
 * Flag to set primary neutrino energy according to San Onofre reactor neutrino
 * spectrum fit. 1 = gives neutrino spectrum in the range 1.8 ~ 10.MeV
 * according to spectrum fit of San Onofre reactor while ignoring
 * PRIMARY_NEUTRINO_KINETIC_ENERGY; 0 = gives mono-energetic neutrinos with
 * energy equal to PRIMARY_NEUTRINO_KINETIC_ENERGY*MeV.
 */
#define REACTOR_NEUTRINO_SPECTRUM_FLAG 0

/*
 * Flag to filter and pick-out only fixed neutron kinetic energies after
 * inverse beta decay. The range to filter is NEUTRON_KINETIC_ENERGY +/-
 * EPSILON.
 */
#define FILTER_NEUTRON_KE 0

using namespace std;
using namespace CLHEP;

// Cross section of neutrino inverse beta decay.
double DiffCrossSection(double Enu, double cosThetaPositronWRTNeutrinoMomentumDirection, double Mn, double Mp, double me, double f, double f2, double g, double cosThetaC, double deltaInnerR, double GFermi);
double GetNeutronMomentum(double Enu, double cosThetaPositronWRTNeutrinoMomentumDirection, double Mn, double Mp, double me);
double GetCosineOfThetaNeutron(double Enu, double cosThetaPositronWRTNeutrinoMomentumDirection, double Mn, double Mp, double me);
double GetPositronMomentum(double Enu, double cosThetaPositronWRTNeutrinoMomentumDirection, double Mn, double Mp, double me);
double GetNeutrinoSpectrum(double Enu);

int main(int argc, char** argv)
{
	const double f = 1.0, f2 = 3.706, g = 1.26; // constants from P. Vogel's paper
	const double cosThetaC = 0.974, deltaInnerR = 0.024; //constants from P. Vogel's paper
	const double me = 0.510999, Mn = 939.565560, Mp = 938.272013; //particle masses (MeV)
	const double GFermi = 1.166371*pow(10.0,-11); //(MeV^-2)
	double cosThetaPositronWRTNeutrinoMomentumDirection, cosThetaNeutronWRTNeutrinoMomentumDirection; //cosThetaPositronWRTNeutrinoMomentumDirection = Cos(theta_e) wrt neutrino momentum direction.
	double KEPositron, KENeutron; // Positron and neutron kinetic energies respectively (MeV).
	double PPositron, PNeutron; // Positron and neutron momentums respectively (MeV).
	ofstream fout ("primaryParticleData.dat");
	const string header = "# event# KE_neutrino(MeV) cos(theta_e+)_wrt_neutrino_momentum_direction KE_e+(MeV) cos(theta_neutron)_wrt_neutrino_momentum_direction KE_neutron(MeV)";
	fout << header << endl;
//	cout << header << endl;
	cout << "Creating file primaryParticleData.dat\n";
	for(int i = 0; i < ITERATIONS;){
		cout << "i = " << i << endl;
#if REACTOR_NEUTRINO_SPECTRUM_FLAG
		double Enu = RandFlat::shoot(1.8, 10.); // Neutrino kinetic energy (MeV).
		double randomSampleValue_1 = RandFlat::shoot(0., 1.);
		if(randomSampleValue_1 > GetNeutrinoSpectrum(Enu)) continue;
#else
		const double Enu = PRIMARY_NEUTRINO_KINETIC_ENERGY; // Constant neutrino kinetic energy (MeV).
#endif
		cosThetaPositronWRTNeutrinoMomentumDirection =	RandFlat::shoot(-1., 1.); // cosThetaPositronWRTNeutrinoMomentumDirection = Cos(theta_e).
		double randomSampleValue_2 = RandFlat::shoot(0., 1.); // Random sample between 0. ~ 1.
		if(randomSampleValue_2 >
				DiffCrossSection(Enu,
					cosThetaPositronWRTNeutrinoMomentumDirection, Mn, Mp, me, f,
					f2, g, cosThetaC, deltaInnerR, GFermi)) continue;
		PPositron = GetPositronMomentum(Enu, cosThetaPositronWRTNeutrinoMomentumDirection, Mn, Mp, me); // Positron momentum in MeV.
		PNeutron = GetNeutronMomentum(Enu, cosThetaPositronWRTNeutrinoMomentumDirection, Mn, Mp, me); // Neutron momentum MeV.
		cosThetaNeutronWRTNeutrinoMomentumDirection = GetCosineOfThetaNeutron(Enu, cosThetaPositronWRTNeutrinoMomentumDirection, Mn, Mp, me); // Cosine theta of neutron angle.
		KEPositron = sqrt(pow(PPositron,2) + pow(me,2)) - me; // Relativistic kinetic energies of particles.
		KENeutron = sqrt(pow(PNeutron,2) + pow(Mn,2)) - Mn;
		fout.precision(10);
#if FILTER_NEUTRON_KE
		if(!(KENeutron > NEUTRON_KINETIC_ENERGY-EPSILON && KENeutron <
					NEUTRON_KINETIC_ENERGY+EPSILON)) continue;
#endif
		fout << i << "\t" << Enu << "\t" << cosThetaPositronWRTNeutrinoMomentumDirection << "\t" << KEPositron/* << "\t" << Ee*/ << "\t" << cosThetaNeutronWRTNeutrinoMomentumDirection << "\t" << KENeutron /*<< "\t" << PPositron << "\t" << PNeutron */<< endl;
		i++;
	}
}
fout.close();
return 0;
}

// Neutrino spectrum shape.
double GetNeutrinoSpectrum(double Enu){
	const double c1 = 0.8;
	const double c2 = 1.43;
	const double c3 = 3.2;
	return pow(Enu-c2, 2)*exp(-pow((Enu+c1)/c3, 2));
}

// Cross section of neutrino inverse beta decay.
double DiffCrossSection(double Enu, double cosThetaPositronWRTNeutrinoMomentumDirection, double Mn, double Mp, double me, double f, double f2, double g, double cosThetaC, double deltaInnerR, double GFermi){
return 3.892729000000001*pow(10.0,19)*(-((pow(cosThetaC,2)*(1 + deltaInnerR)*pow(GFermi,2)*(Enu - Mn + Mp)*sqrt(-pow(me,2) + pow(Enu - Mn + Mp,2))*
						((pow(f,2) - pow(g,2))*cosThetaPositronWRTNeutrinoMomentumDirection*(sqrt(-pow(me,2) + pow(Enu - Mn + Mp,2)) - Enu*cosThetaPositronWRTNeutrinoMomentumDirection) + (pow(f,2) + 3*pow(g,2))*(-Mn + Mp + Enu*(1 - ((Enu - Mn + Mp)*cosThetaPositronWRTNeutrinoMomentumDirection)/sqrt(-pow(me,2) + pow(Enu - Mn + Mp,2)))) + 
						 2*(f + f2)*g*(-(pow(me,2)/(Enu - Mn + Mp)) + (2*Enu - Mn + Mp)*(1 - (sqrt(-pow(me,2) + pow(Enu - Mn + Mp,2))*cosThetaPositronWRTNeutrinoMomentumDirection)/(Enu - Mn + Mp))) + 
						 (pow(f,2) + pow(g,2))*(pow(me,2)/(Enu - Mn + Mp) + (Mn - Mp)*(1 + (sqrt(-pow(me,2) + pow(Enu - Mn + Mp,2))*cosThetaPositronWRTNeutrinoMomentumDirection)/(Enu - Mn + Mp)))))/((Mn + Mp)*pi)) + 
				(pow(cosThetaC,2)*(1 + deltaInnerR)*pow(GFermi,2)*(-2*pow(Enu,2) + pow(me,2) + 3*Enu*Mn - 2*pow(Mn,2) - Enu*Mp + 2*Mn*Mp + 2*Enu*sqrt(-pow(me,2) + pow(Enu - Mn + Mp,2))*cosThetaPositronWRTNeutrinoMomentumDirection)*
				 (Enu*pow(me,2)*cosThetaPositronWRTNeutrinoMomentumDirection - Enu*pow(Mn,2)*cosThetaPositronWRTNeutrinoMomentumDirection + 2*pow(Enu,2)*Mp*cosThetaPositronWRTNeutrinoMomentumDirection + Enu*pow(Mp,2)*cosThetaPositronWRTNeutrinoMomentumDirection + 
				  sqrt(pow(Enu + Mp,2)*(pow(me,4) + pow(pow(Mn,2) - pow(Mp,2),2) - 4*Enu*Mp*(pow(me,2) + pow(Mn,2) - pow(Mp,2)) - 2*pow(me,2)*(pow(Mn,2) + pow(Mp,2)) + 
						  4*pow(Enu,2)*(pow(Mp,2) + pow(me,2)*(-1 + pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))))))*(pow(f,2) + 3*pow(g,2) + 
				  ((pow(f,2) - pow(g,2))*(Mn + Mp)*cosThetaPositronWRTNeutrinoMomentumDirection*(-(Enu*pow(me,2)*cosThetaPositronWRTNeutrinoMomentumDirection) + Enu*pow(Mn,2)*cosThetaPositronWRTNeutrinoMomentumDirection - 2*pow(Enu,2)*Mp*cosThetaPositronWRTNeutrinoMomentumDirection - Enu*pow(Mp,2)*cosThetaPositronWRTNeutrinoMomentumDirection - 
					  sqrt(pow(Enu + Mp,2)*(pow(me,4) + pow(pow(Mn,2) - pow(Mp,2),2) - 4*Enu*Mp*(pow(me,2) + pow(Mn,2) - pow(Mp,2)) - 2*pow(me,2)*(pow(Mn,2) + pow(Mp,2)) + 
							  4*pow(Enu,2)*(pow(Mp,2) + pow(me,2)*(-1 + pow(cosThetaPositronWRTNeutrinoMomentumDirection,2)))))))/
				  (2.*(2*pow(Enu,2) - pow(me,2) - 3*Enu*Mn + 2*pow(Mn,2) + Enu*Mp - 2*Mn*Mp - 2*Enu*sqrt(-pow(me,2) + pow(Enu - Mn + Mp,2))*cosThetaPositronWRTNeutrinoMomentumDirection)*(2*Enu*Mp + pow(Mp,2) - pow(Enu,2)*(-1 + pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))))))/
				(4.*(Mn + Mp)*pi*(2*Enu*Mp + pow(Mp,2) - pow(Enu,2)*(-1 + pow(cosThetaPositronWRTNeutrinoMomentumDirection,2)))));
}

// Positron momentum in MeV.
double GetPositronMomentum(double Enu, double cosThetaPositronWRTNeutrinoMomentumDirection, double Mn, double Mp, double me){
			return (-(Enu*(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 4*pow(Mp,2))*cosThetaPositronWRTNeutrinoMomentumDirection) - 
					sqrt(pow(Enu,2)*pow(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 
							4*pow(Mp,2),2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2) - 
						4*(-4*pow(Enu,2)*pow(me,2) + pow(me,4) - 
							2*pow(me,2)*pow(Mn,2) + pow(Mn,4) - 4*Enu*pow(me,2)*Mp - 
							4*Enu*pow(Mn,2)*Mp + 4*pow(Enu,2)*pow(Mp,2) - 
							2*pow(me,2)*pow(Mp,2) - 2*pow(Mn,2)*pow(Mp,2) + 
							4*Enu*pow(Mp,3) + pow(Mp,4))*
						(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
						 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))))/
				(2.*(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
					 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2)));
}

// Neutron momentum in MeV.
double GetNeutronMomentum(double Enu, double cosThetaPositronWRTNeutrinoMomentumDirection, double Mn, double Mp, double me){
			return sqrt(pow(Enu,2) - (Enu*cosThetaPositronWRTNeutrinoMomentumDirection*(-(Enu*
								(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 4*pow(Mp,2))*cosThetaPositronWRTNeutrinoMomentumDirection) - 
							sqrt(pow(Enu,2)*pow(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 
									4*pow(Mp,2),2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2) - 
								4*(-4*pow(Enu,2)*pow(me,2) + pow(me,4) - 
									2*pow(me,2)*pow(Mn,2) + pow(Mn,4) - 
									4*Enu*pow(me,2)*Mp - 4*Enu*pow(Mn,2)*Mp + 
									4*pow(Enu,2)*pow(Mp,2) - 2*pow(me,2)*pow(Mp,2) - 
									2*pow(Mn,2)*pow(Mp,2) + 4*Enu*pow(Mp,3) + pow(Mp,4))*
								(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
								 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2)))))/
					(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))\
					+ (pow(cosThetaPositronWRTNeutrinoMomentumDirection,2)*pow(-(Enu*(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 
									4*pow(Mp,2))*cosThetaPositronWRTNeutrinoMomentumDirection) - 
							sqrt(pow(Enu,2)*pow(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 
									4*pow(Mp,2),2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2) - 
								4*(-4*pow(Enu,2)*pow(me,2) + pow(me,4) - 
									2*pow(me,2)*pow(Mn,2) + pow(Mn,4) - 
									4*Enu*pow(me,2)*Mp - 4*Enu*pow(Mn,2)*Mp + 
									4*pow(Enu,2)*pow(Mp,2) - 2*pow(me,2)*pow(Mp,2) - 
									2*pow(Mn,2)*pow(Mp,2) + 4*Enu*pow(Mp,3) + pow(Mp,4))*
								(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
								 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))),2))/
					(4.*pow(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
							  4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2),2)) + 
					((1 - pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))*pow(-(Enu*
											  (4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 4*pow(Mp,2))*cosThetaPositronWRTNeutrinoMomentumDirection) - 
											sqrt(pow(Enu,2)*pow(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 
													4*pow(Mp,2),2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2) - 
												4*(-4*pow(Enu,2)*pow(me,2) + pow(me,4) - 
													2*pow(me,2)*pow(Mn,2) + pow(Mn,4) - 
													4*Enu*pow(me,2)*Mp - 4*Enu*pow(Mn,2)*Mp + 
													4*pow(Enu,2)*pow(Mp,2) - 2*pow(me,2)*pow(Mp,2) - 
													2*pow(Mn,2)*pow(Mp,2) + 4*Enu*pow(Mp,3) + pow(Mp,4))*
												(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
												 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))),2))/
					(4.*pow(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
							  4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2),2)));
}

// Cosine theta of neutron angle wrt neutrino momentum direction.
double GetCosineOfThetaNeutron(double Enu, double cosThetaPositronWRTNeutrinoMomentumDirection, double Mn, double Mp, double me){
   return (Enu - (cosThetaPositronWRTNeutrinoMomentumDirection*(-(Enu*(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 4*pow(Mp,2))*
								cosThetaPositronWRTNeutrinoMomentumDirection) - sqrt(pow(Enu,2)*
									pow(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 4*pow(Mp,2),
										2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2) - 4*
									(-4*pow(Enu,2)*pow(me,2) + pow(me,4) - 
									 2*pow(me,2)*pow(Mn,2) + pow(Mn,4) - 
									 4*Enu*pow(me,2)*Mp - 4*Enu*pow(Mn,2)*Mp + 
									 4*pow(Enu,2)*pow(Mp,2) - 2*pow(me,2)*pow(Mp,2) - 
									 2*pow(Mn,2)*pow(Mp,2) + 4*Enu*pow(Mp,3) + pow(Mp,4))*
									(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
									 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2)))))/
					(2.*(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
						 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))))/
				sqrt(pow(Enu,2) - (Enu*cosThetaPositronWRTNeutrinoMomentumDirection*(-(Enu*
									(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 4*pow(Mp,2))*cosThetaPositronWRTNeutrinoMomentumDirection) - 
								sqrt(pow(Enu,2)*pow(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 
										4*pow(Mp,2),2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2) - 
									4*(-4*pow(Enu,2)*pow(me,2) + pow(me,4) - 
										2*pow(me,2)*pow(Mn,2) + pow(Mn,4) - 
										4*Enu*pow(me,2)*Mp - 4*Enu*pow(Mn,2)*Mp + 
										4*pow(Enu,2)*pow(Mp,2) - 2*pow(me,2)*pow(Mp,2) - 
										2*pow(Mn,2)*pow(Mp,2) + 4*Enu*pow(Mp,3) + pow(Mp,4))*
									(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
									 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2)))))/
						(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))
						+ (pow(cosThetaPositronWRTNeutrinoMomentumDirection,2)*pow(-(Enu*
									(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 4*pow(Mp,2))*cosThetaPositronWRTNeutrinoMomentumDirection) - 
								sqrt(pow(Enu,2)*pow(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 
										4*pow(Mp,2),2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2) - 
									4*(-4*pow(Enu,2)*pow(me,2) + pow(me,4) - 
										2*pow(me,2)*pow(Mn,2) + pow(Mn,4) - 
										4*Enu*pow(me,2)*Mp - 4*Enu*pow(Mn,2)*Mp + 
										4*pow(Enu,2)*pow(Mp,2) - 2*pow(me,2)*pow(Mp,2) - 
										2*pow(Mn,2)*pow(Mp,2) + 4*Enu*pow(Mp,3) + pow(Mp,4))*
									(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
									 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))),2))/
						(4.*pow(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
								  4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2),2)) + 
						((1 - pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))*pow(-(Enu*
												  (4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 4*pow(Mp,2))*cosThetaPositronWRTNeutrinoMomentumDirection) - 
												sqrt(pow(Enu,2)*pow(4*pow(me,2) - 4*pow(Mn,2) + 8*Enu*Mp + 
														4*pow(Mp,2),2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2) - 
													4*(-4*pow(Enu,2)*pow(me,2) + pow(me,4) - 
														2*pow(me,2)*pow(Mn,2) + pow(Mn,4) - 
														4*Enu*pow(me,2)*Mp - 4*Enu*pow(Mn,2)*Mp + 
														4*pow(Enu,2)*pow(Mp,2) - 2*pow(me,2)*pow(Mp,2) - 
														2*pow(Mn,2)*pow(Mp,2) + 4*Enu*pow(Mp,3) + pow(Mp,4))*
													(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
													 4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2))),2))/
						(4.*pow(-4*pow(Enu,2) - 8*Enu*Mp - 4*pow(Mp,2) + 
								  4*pow(Enu,2)*pow(cosThetaPositronWRTNeutrinoMomentumDirection,2),2)));
}
