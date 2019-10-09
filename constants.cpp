// constants.cpp

#include "constants.hpp"
#include "Zaimoni.STL/Logging.h"

#include <cmath>	// do not need augmentations here

// note that CODATA estimates are released the year after their name (they are named after the cutoff point which is usually late December)
// so e.g. wikipedia commentary on the changes is under CODATA 2019
#ifndef CODATA_VERSION
#define CODATA_VERSION 2018
#endif

// interval entries done at +/1 standard deviation [sic]

#if CODATA_VERSION==2018
const fundamental_constants::interval fundamental_constants::N_A(6.02214076e23);	// mol^-1; definition
const fundamental_constants::interval fundamental_constants::inv_alpha(137.035999063, 137.035999105);
const fundamental_constants::interval fundamental_constants::alpha(7.2973525682e-3, 7.2973525704e-3);	// electron-charge^2/[(4pi epsilon_0 h-bar c]
#else
const fundamental_constants::interval fundamental_constants::N_A(6.022140787e23, 6.022140931e23);	// CODATA 2014; mol^-1	
const fundamental_constants::interval fundamental_constants::inv_alpha(137.035999108, 137.035999170);	// CODATA 2014
const fundamental_constants::interval fundamental_constants::alpha(7.2973525627e-3, 7.2973525661e-3);	// electron-charge^2/[(4pi epsilon_0 h-bar c]
#endif

// CODATA 2010
#define SI_CODATA_C 299792458.0
#define SI_CODATA_CS133_HYPERFINE_HZ 9192631770.0
#define SI_TO_CGS_DISTANCE_SCALE 100.0

// default-initialize to SI i.e. MKS
fundamental_constants::fundamental_constants()
:	distance_unit(1.0),
	time_unit(1.0),
	mass_unit(1.0),
	temperature_unit(1.0),
	charge_unit(1.0),
 	c(SI_CODATA_C),	// CODATA 2010/2014/2018; m/s
#if CODATA_VERSION==2018
	G(6.67400e-11, 6.67460e-11),	// CODATA 2018; m^3 kg^-1 s^-2
	k(1.380649e-23),	//	CODATA 2018 (actually 2019)
	h_bar(interval(6.62607015e-34)/interval(2.0*interval_shim::pi)),	// CODATA 2019 (h definition)
#elif CODATA_VERSION==2010
	G(6.67304e-11,6.67464e-11),	// CODATA 2010; m^3 kg^-1 s^-2
	k(1.3806475e-23,1.3806501e-23),	// CODATA 2010; J/K i.e. m^2 kg s^-2 K^-1
	h_bar(1.054571679e-34,1.054571773e-34)	// CODATA 2010; J s i.e. m^2 kg s^-1
#else
	G(6.67377e-11, 6.67439e-11),	// CODATA 2014; m^3 kg^-1 s^-2
	k(1.38064773e-23, 1.38064921e-23),	// CODATA 2014; J/K i.e. m^2 kg s^-2 K^-1
	h_bar(1.054571787e-34, 1.054571813e-34),	// CODATA 2014; J s i.e. m^2 kg s^-1
#endif
	// atomic units
#if CODATA_VERSION==2018
	amu_mass(1.66053906560e-27, 1.66053906760e-27),	// CODATA 2018; kg 1.660 539 066 60 e-27    0.000 000 000 50 e-27
	Q_e(1.602176634e-19)	// CODATA 2019 (definition); C
#elif CODATA_VERSION==2010
	amu_mass(1.660538775e-27, 1.660539018e-27),	// 2010 CODATA: 1.660 538 921 e-27       0.000 000 073 e-27
	Q_e(1.6021766110e-19, 1.6021766306e-19)	// CODATA 2014; C	// should fix this
#else
	amu_mass(1.660539000e-27, 1.660539080e-27),	// 2014 CODATA: 1.660 539 040 e - 27       0.000 000 020 e - 27
	Q_e(1.6021766110e-19, 1.6021766306e-19)	// CODATA 2014; C
#endif
#if 0
	m_e(9.10938345e-31, 9.10938367e-31),	// CODATA 2014; kg
	mu_0(4e-7*interval_shim::pi),	// CODATA 2014; Ampere definition implies 4pi*10^-7 H/m i.e. N A^-2
	epsilon_0(1.0/(mu_0*square(c)))			// mu_0*epsilon_0 = c^-2 from Maxwell equations; F/m; F := s^4 A^2 m^-2 kg^-1; alternately s^2/H
	// in CODATA 2018/2019, epsilon_0 is computed from the fine structure constant as the other quantities are defined
	// i.e. classical electrostatic/electrodynamic problems may be cleaner in CODATA 2014 than CODATA 2018
#endif
{
}

void fundamental_constants::mult_scale_distance(interval x)
{
	const interval x2(square(x));
	distance_unit /= x;
	c *= x;
	G *= x;
	G *= x2;
	k *= x2;
	h_bar *= x2;
}

void fundamental_constants::div_scale_distance(interval x)
{
	const interval x2(square(x));
	distance_unit *= x;
	c /= x;
	G /= x;
	G /= x2;
	k /= x2;
	h_bar /= x2;
}

void fundamental_constants::mult_scale_time(interval x)
{
	const interval x2(square(x));
	time_unit /= x;
	c /= x;
	G /= x2;
	k /= x2;
	h_bar /= x;
}

void fundamental_constants::div_scale_time(interval x)
{
	const interval x2(square(x));
	time_unit *= x;
	c *= x;
	G *= x2;
	k *= x2;
	h_bar *= x;
}

void fundamental_constants::mult_scale_mass(interval x)
{
	mass_unit /= x;
	amu_mass /= x;
	G /= x;
	k *= x;
	h_bar *= x;
}

void fundamental_constants::div_scale_mass(interval x)
{
	mass_unit *= x;
	amu_mass *= x;
	G *= x;
	k /= x;
	h_bar /= x;
}

void fundamental_constants::mult_scale_temperature(interval x)
{
	temperature_unit /= x;
	k /= x;
}

void fundamental_constants::div_scale_temperature(interval x)
{
	temperature_unit *= x;
	k *= x;
}

void fundamental_constants::mult_scale_charge(interval x)
{
	charge_unit /= x;
	Q_e /= x;
}

void fundamental_constants::div_scale_charge(interval x)
{
	charge_unit *= x;
	Q_e *= x;
}


void fundamental_constants::geometrize()
{
/*
: 	c(299792458.0),	// CODATA 2010; m/s
	G(6.67304e-11,6.67464e-11),	// CODATA 2010; m^3 kg^-1 s^-2
	k(1.3806475e-23,1.3806501e-23),	// CODATA 2010; J/K i.e. m^2 kg s^-2 K^-1
	h_bar(1.054571679e-34,1.054571773e-34)	// CODATA 2010; J s i.e. m^2 kg s^-1

    geometrized:
    1 = dist time^-1
    1 = dist^3 mass^-1 time^-2
    1 = dist^2 mass time^-2 temperature^-1
    1 = dist^2 mass time^-1

    dimensions of
    G/c^2: dist mass^-1
    k/c^2: mass temperature^-1
    c^2/k: temperature mass^-1
    h_bar/c : dist mass

    G*h_bar/c^3: dist^2
    h_bar*c/G: mass^2   
 */
	const interval geo_dist_squared_1 = (G/pow(c,3))*h_bar;
	const interval geo_dist_squared_2 = (h_bar/pow(c,3))*G;
	const interval geo_dist_squared_3 = (G*h_bar)/pow(c,3);
	const interval geo_dist_squared = intersect(intersect(geo_dist_squared_1,geo_dist_squared_2),geo_dist_squared_3);

	const interval geo_time_squared_1 = (G/pow(c,5))*h_bar;
	const interval geo_time_squared_2 = (h_bar/pow(c,5))*G;
	const interval geo_time_squared_3 = (G*h_bar)/pow(c,5);
	const interval geo_time_squared = intersect(intersect(geo_time_squared_1,geo_time_squared_2),geo_time_squared_3);

	const interval geo_mass_squared_1 = (h_bar/G)*c;
	const interval geo_mass_squared_2 = (c/G)*h_bar;
	const interval geo_mass_squared_3 = (h_bar*c)/G;
	const interval geo_mass_squared = intersect(intersect(geo_mass_squared_1,geo_mass_squared_2),geo_mass_squared_3);

	const interval geo_temperature_1 = (square(c)/k)*sqrt(geo_mass_squared);
	const interval geo_temperature_2 = (sqrt(geo_mass_squared)/k)*square(c);
	const interval geo_temperature_3 = (square(c)*sqrt(geo_mass_squared))/k;
	const interval geo_temperature = intersect(intersect(geo_temperature_1,geo_temperature_2),geo_temperature_3);

	div_scale_distance(sqrt(geo_dist_squared));
	div_scale_time(sqrt(geo_time_squared));
	div_scale_mass(sqrt(geo_mass_squared));
	div_scale_temperature(geo_temperature);

	// set geometrized constants to 1
	c = 1.0;
	G = 1.0;
	k = 1.0;
	h_bar = 1.0;

	// we do not include electric charge in geometrization because there is no valid consensus: 
	// lore is that one must choose between a clean force law, and the electron having unit electric charge.
	// this policy can be changed once some test cases are available.
	// note that a clean force law equates elecrostatic and electromagnetic charge units (cf CGS vs MKS issues)
	// so maybe the problem can be shoved into epsilon_0?
}

const fundamental_constants& SI_units()
{
	static fundamental_constants* x = NULL;
	if (x) return *x;
	x = new fundamental_constants();
	return *x;
}

const fundamental_constants& CGS_units()
{
	static fundamental_constants* x = NULL;
	if (x) return *x;
	x = new fundamental_constants();
	x->mult_scale_distance(100.0);	// 100 cm to 1 m
	x->mult_scale_mass(1000.0);	// 1000 g to 1 kg
	// CGS unit of charge does not have same dimensionality as MKS and geometrized systems.  Following is the electrostatic conversion to statcoulombs
	x->mult_scale_charge(10*SI_CODATA_C);	// mass^1/2 length^3/2 time^-1 (!) due force law F = q1q2/r^2 rather than F = q1q1/[4pi epsilon_0 r^2]
	// electromagnetic version has an extra factor of 4pi; same dimensionality, however
	// The electrostatic conversion is exact only when epsilon_0 is an exact constant by construction (CODATA 2014-).
	return *x;
}

const fundamental_constants& geometrized_units()
{
	static fundamental_constants* x = NULL;
	if (x) return *x;
	x = new fundamental_constants();
	x->geometrize();
	return *x;
}

const fundamental_constants& solar_system_units()
{
	static fundamental_constants* x = NULL;
	if (x) return *x;
	x = new fundamental_constants();
	const fundamental_constants::interval SI_G(x->G);

	// time unit: 1 Earth year
	// distance unit: 1 AU (semimajor axis of Earth's orbit)
	// * 2012: 1 AU := 149,597,870,700 meters
	// mass unit: sum of Earth and Sun rest masses (or in practice, just the sun's rest mass)
	// celestial mechanics: gravitational parameter GM
	// for the earth and sun, this is known to very high precision
	// https://en.wikipedia.org/wiki/Standard_gravitational_parameter
	// geocentric (Earth) : 398600.4418�0.0008 km3 s-2
	// heliocentric (Sun) : 1.32712440018 x 10^20 (� 8 x 10^9) m3 s-2 http://ssd.jpl.nasa.gov/?constants
	
	x->div_scale_distance(1.495978707e11);	// AU definition
	x->div_scale_time(86400*365.25636);	// sidereal year, quasar reference frame
	x->div_scale_mass(fundamental_constants::interval(1.32712440010e20,1.32712440026e20)/SI_G);
	return *x;
}

const fundamental_constants& fundamental_constants::get(units src)
{
	switch (src)
	{
	case MKS: return SI_units();
	case CGS: return CGS_units();
	case PLANCK: return geometrized_units();
	default: _fatal_code("unhandled unit system code", 3);
	}
}

#ifdef TEST_APP
// example build line
// g++ -std=c++14 -oconstants.exe -D__STDC_LIMIT_MACROS -DTEST_APP constants.cpp -Llib\host.isk -lz_stdio_c -lz_log_adapter -lz_stdio_log -lz_format_util

#include "test_driver.h"

int main(int argc, char* argv[])
{	// parse options
	char buf[100];

	STRING_LITERAL_TO_STDOUT("starting main\n");

	STRING_LITERAL_TO_STDOUT("dimensionless: inv-alpha and alpha\n");
	INTERVAL_TO_STDOUT(fundamental_constants::inv_alpha, "\n");
	INTERVAL_TO_STDOUT(1.0/fundamental_constants::alpha, "\n");
	INTERVAL_TO_STDOUT(1.0/fundamental_constants::inv_alpha, "\n");
	INTERVAL_TO_STDOUT(fundamental_constants::alpha, "\n");

	STRING_LITERAL_TO_STDOUT("\npi\n");
	INTERVAL_TO_STDOUT(interval_shim::pi, " \n");

	STRING_LITERAL_TO_STDOUT("\nspeed of light\n");
	INTERVAL_TO_STDOUT(SI_units().c," m/s\n");
	INTERVAL_TO_STDOUT(CGS_units().c," cm/s\n");
	INTERVAL_TO_STDOUT(geometrized_units().c," geometrized distance/time\n");
	INTERVAL_TO_STDOUT(solar_system_units().c," AU/sidereal year\n");

	STRING_LITERAL_TO_STDOUT("\nNewtonian G\n");
	INTERVAL_TO_STDOUT(SI_units().G,"\n");
	INTERVAL_TO_STDOUT(CGS_units().G,"\n");
	INTERVAL_TO_STDOUT(geometrized_units().G,"\n");
	INTERVAL_TO_STDOUT(solar_system_units().G,"\n");

	STRING_LITERAL_TO_STDOUT("\nBoltzmann constant k\n");
	INTERVAL_TO_STDOUT(SI_units().k,"\n");
	INTERVAL_TO_STDOUT(CGS_units().k,"\n");
	INTERVAL_TO_STDOUT(geometrized_units().k,"\n");
	INTERVAL_TO_STDOUT(solar_system_units().k,"\n");

	STRING_LITERAL_TO_STDOUT("\nPlanck constant over 2*pi h_bar\n");
	INTERVAL_TO_STDOUT(SI_units().h_bar,"\n");
	INTERVAL_TO_STDOUT(CGS_units().h_bar,"\n");
	INTERVAL_TO_STDOUT(geometrized_units().h_bar,"\n");
	INTERVAL_TO_STDOUT(solar_system_units().h_bar,"\n");

	STRING_LITERAL_TO_STDOUT("\nIAU units in SI\n");
	INTERVAL_TO_STDOUT(solar_system_units().distance_unit," m\n");
	INTERVAL_TO_STDOUT(solar_system_units().time_unit," s\n");
	INTERVAL_TO_STDOUT(solar_system_units().mass_unit," kg\n");
	INTERVAL_TO_STDOUT(solar_system_units().temperature_unit," K\n");

	STRING_LITERAL_TO_STDOUT("\nGeometrized units in SI\n");
	INTERVAL_TO_STDOUT(geometrized_units().distance_unit," m\n");
	INTERVAL_TO_STDOUT(geometrized_units().time_unit," s\n");
	INTERVAL_TO_STDOUT(geometrized_units().mass_unit," kg\n");
	INTERVAL_TO_STDOUT(geometrized_units().temperature_unit," K\n");

	const fundamental_constants::interval geo_dist_squared = SI_units().G*SI_units().h_bar/(SI_units().c*square(SI_units().c));
	STRING_LITERAL_TO_STDOUT("\nGeometrized distance unit squared\n");
	INTERVAL_TO_STDOUT(geo_dist_squared," m^2\n");

	const fundamental_constants::interval geo_time_squared = geo_dist_squared/square(SI_units().c);
	STRING_LITERAL_TO_STDOUT("\nGeometrized time unit squared\n");
	INTERVAL_TO_STDOUT(geo_time_squared," s^2\n");

	const fundamental_constants::interval geo_mass_squared = SI_units().h_bar*SI_units().c/SI_units().G;
	STRING_LITERAL_TO_STDOUT("\nGeometrized mass unit squared\n");
	INTERVAL_TO_STDOUT(geo_mass_squared," kg^2\n");

	const fundamental_constants::interval geo_temperature = square(SI_units().c)/SI_units().k*sqrt(geo_mass_squared);
	STRING_LITERAL_TO_STDOUT("\nGeometrized temperature unit squared\n");
	INTERVAL_TO_STDOUT(geo_temperature," K\n");

	fundamental_constants test_geometrization;
	test_geometrization.div_scale_distance(sqrt(geo_dist_squared));
	test_geometrization.div_scale_time(sqrt(geo_time_squared));
	test_geometrization.div_scale_mass(sqrt(geo_mass_squared));
	test_geometrization.div_scale_temperature(geo_temperature);
	STRING_LITERAL_TO_STDOUT("\nCross-check geometrization\n");
	INTERVAL_TO_STDOUT(test_geometrization.c,"\n");
	INTERVAL_TO_STDOUT(test_geometrization.G,"\n");
	INTERVAL_TO_STDOUT(test_geometrization.k,"\n");
	INTERVAL_TO_STDOUT(test_geometrization.h_bar,"\n");	

	STRING_LITERAL_TO_STDOUT("tests finished\n");
}

#endif
