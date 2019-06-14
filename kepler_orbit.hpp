#ifndef KEPLER_ORBIT_HPP
#define KEPLER_ORBIT_HPP

#include "Zaimoni.STL/Logging.h"
#include "mass.hpp"
#include "conic.hpp"
#include "coord_chart.hpp"

namespace kepler {

// implied coordinate system is perifocal: https://en.wikipedia.org/wiki/Perifocal_coordinate_system
// for the Newtonian two-body problem, this is the same as barycentric coordinates
// also cf https://en.wikipedia.org/wiki/Apsis and https://en.wikipedia.org/wiki/Kepler%27s_laws_of_planetary_motion

// the periapsis (perihelion, etc.) is on the positive x-axis
// y-axis unit vector is at theta=90 degrees in polar coordinates
// when embedding in 3-space, the unit vector parallel to the angular momentum vector is the cross-product of the above two vectors.

class orbit
{
public:
	typedef fundamental_constants::interval interval;
	typedef zaimoni::math::Cartesian_vector<interval, 2>::coord_type vector;
	typedef zaimoni::math::conic conic;
	typedef zaimoni::circle::angle angle;
private:
	mass _m;	// usually specified as the reduced gravitational parameter.  Provides the unit system in use.
	conic _orbit;	// the actual orbit
	// while we can calculate the orbit from the perihelion/aphelion pair and vice versa,
	// we do want to record both as the construction parameters actually used are considered more accurate
	interval _pericenter;	// barycentric perihelion, etc.
	interval _apocenter;	/// barycentric aphelion, etc.
	// following cache variables do not actually need to reach the savefile
	mutable bool have_m_div_a = false;
	mutable interval _m_div_a;
	mutable bool have_one_minus_e_div_one_plus_e = false;
	mutable interval _one_minus_e_div_one_plus_e;
	mutable bool have_m_div_specific_angular_momentum = false;
	mutable interval _m_div_specific_angular_momentum;
	mutable bool have_mean_anomaly_scale = false;
	mutable angle _mean_anomaly_scale;
public:
	orbit() = default;
	orbit(const orbit& src) = default;
	orbit(const mass& m, const conic& o) : _m(m), _orbit(o), _pericenter((1.0-o.e())*o.a()), _apocenter((1.0 + o.e())* o.a()) {};
	orbit(const mass& m, const interval& barycentric_perihelion, const interval& barycentric_aphelion) : _m(m), _orbit(_from_perihelion_aphelion(barycentric_perihelion, barycentric_aphelion)), _pericenter(barycentric_perihelion), _apocenter(barycentric_aphelion) {};

	~orbit() = default;
	orbit& operator=(const orbit& src) = default;

	friend bool operator==(const orbit& lhs, const orbit& rhs) {
		return lhs._m == rhs._m && lhs._orbit == rhs._orbit && lhs._pericenter == rhs._pericenter && lhs._apocenter == rhs._apocenter;
	}
	friend bool operator!=(const orbit& lhs, const orbit& rhs) { return !(lhs == rhs); }

	// accessors
	const mass& m() const { return _m; }
	const conic& o() const { return _orbit; }
	const interval& pericenter() const { return _pericenter; }
	const interval& apocenter() const { return _apocenter; }

	const interval& m_div_a() const {
		if (have_m_div_a) return _m_div_a;
		have_m_div_a = true;
		return (_m_div_a = _m.GM() / _orbit.a());
	}

	const interval& one_minus_e_div_one_plus_e() const {
		if (have_one_minus_e_div_one_plus_e) return _one_minus_e_div_one_plus_e;
		have_one_minus_e_div_one_plus_e = true;
		return (_one_minus_e_div_one_plus_e = (1.0 - _orbit.e()) / (1.0 + _orbit.e()));	// numerator wants support from conic class
	}

	const interval& m_div_specific_angular_momentum() const {
		if (have_m_div_specific_angular_momentum) return _m_div_specific_angular_momentum;
		have_m_div_specific_angular_momentum = true;
		return (_m_div_specific_angular_momentum = _m.GM() / specific_relative_angular_momentum());
	}

	const angle& mean_anomaly_scale() const {
		if (have_mean_anomaly_scale) return _mean_anomaly_scale;
		have_mean_anomaly_scale = true;
		return (_mean_anomaly_scale = angle(360.0 / sqrt(period_squared())));
	}

	interval v_pericenter() const { return sqrt(m_div_a() / one_minus_e_div_one_plus_e()); }
	interval v_apocenter() const { return sqrt(m_div_a() * one_minus_e_div_one_plus_e()); }
	interval specific_orbital_energy() const { return -0.5*m_div_a(); }
	interval specific_relative_angular_momentum() const { return sqrt((1.0 - square(_orbit.e()) * _m.GM() * _orbit.a())); }	// again, wants support from conic class
	interval geometric_mean_of_v_pericenter_v_apocenter() const { return sqrt(m_div_a()); }
	interval predicted_e() const { return (_apocenter-_pericenter) / (_apocenter + _pericenter); }	// some data normalization at construction time indicated

//	vector r(const angle& true_anomaly) {}
	vector v(const angle& true_anomaly) {
		interval _sin;
		interval _cos;
		true_anomaly.sincos(_sin, _cos);
		interval tmp = m_div_specific_angular_momentum();
		interval vec[2] = { -_sin * tmp , tmp * (_orbit.e() + _cos) };
		return vector(vec);
	}
	interval d_polar_r_dt(const angle& true_anomaly) const {
		interval _sin;
		interval _cos;
		true_anomaly.sincos(_sin, _cos);
		return m_div_specific_angular_momentum()*_orbit.e()*_sin;
	}
	interval polar_r(const angle& eccentric_anomaly) const {
		interval _sin;
		interval _cos;
		eccentric_anomaly.sincos(_sin, _cos);
		return _orbit.a()*(1.0-_orbit.e()* _cos);
	}
/*
	interval polar_theta(const angle& eccentric_anomaly) const {	// i.e. true anomaly
		// numerically solve: (1-_orbit.e())tan^2(theta) = (1+_orbit.e()) tan^2(eccentric_anomaly)
		// obviously need an alternate expression for near 90 degrees/270 degrees
	}
	zaimoni::math::angle eccentric_anomaly(const angle& mean_anomaly) {
		// numerically solve: mean_anomaly = E - _orbit.e()*sin(E) // requires working in radians?
		// test with parabolic orbit (there should be limiting values for the eccentric anomaly as time goes to infinity
	}
*/
	interval period_squared() const { return square(2.0 * interval_shim::pi) * pow(_orbit.a(), 3) / _m.GM(); }	// dimension time^2
	angle mean_anomaly(const interval& t) { return mean_anomaly_scale() * t; }

private:
	static conic _from_perihelion_aphelion(const interval& barycentric_perihelion, const interval& barycentric_aphelion) {
		interval major = (barycentric_perihelion + barycentric_aphelion) / 2.0;
		interval minor = sqrt(barycentric_perihelion * barycentric_aphelion);
		return conic(zaimoni::math::conic_tags::ellipse(), major, minor);
	}
};

}	// namespace kepler

#endif