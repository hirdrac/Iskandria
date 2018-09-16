// lossy.hpp

#ifndef LOSSY_HPP
#define LOSSY_HPP 1

#include <boost/numeric/interval.hpp>

#include "Zaimoni.STL/augment.STL/cmath"
#include "Zaimoni.STL/Logging.h"

template<class T>
inline void INFORM(const boost::numeric::interval<T>& x)
{
	if (x.lower()==x.upper())
		{
		INFORM(x.upper());
		return;
		}
	INC_INFORM("[");
	INC_INFORM(x.lower());
	INC_INFORM(",");
	INC_INFORM(x.upper());
	INFORM("]");
}

template<class T>
inline void INC_INFORM(const boost::numeric::interval<T>& x)
{
	if (x.lower()==x.upper())
		{
		INC_INFORM(x.upper());
		return;
		}
	INC_INFORM("[");
	INC_INFORM(x.lower());
	INC_INFORM(",");
	INC_INFORM(x.upper());
	INC_INFORM("]");
}

namespace zaimoni {

template<class T>
constexpr bool isFinite(const boost::numeric::interval<T>& x)
{
	return isFinite(x.lower()) && isFinite(x.upper());
}

template<class T>
constexpr bool isNaN(const boost::numeric::interval<T>& x)
{
	return isNaN(x.lower()) || isNaN(x.upper());
}

template<class T>
constexpr boost::numeric::interval<T> scalBn(const boost::numeric::interval<T>& x,int scale)
{
	return boost::numeric::interval<T>(scalBn(x.lower(),scale),scalBn(x.upper(),scale));
}

namespace math {

using zaimoni::isNaN;
using zaimoni::scalBn;

// testing indicates Boost's numeric interval Does The Wrong Thing here (crashes sin by div-by-zero)
template<>
template<class T>
struct static_cache<boost::numeric::interval<T> >
{
	template<intmax_t n> 
	static const boost::numeric::interval<T>& as()
	{
		static const boost::numeric::interval<T> ret((T)n);
		return ret;
	}

	template<uintmax_t n> 
	static typename std::enable_if<
			std::numeric_limits<intmax_t>::max()<n,
		const boost::numeric::interval<T>& >::type as2()
	{
		static const boost::numeric::interval<T> ret((T)n);
		return ret;
	}

	static boost::numeric::interval<T> as3(intmax_t n)
	{
		return boost::numeric::interval<T>(T(n));
	}

	static boost::numeric::interval<T> as4(uintmax_t n)
	{
		return boost::numeric::interval<T>(T(n));
	}
};


// identify interval-arithmetic type suitable for degrading to
// default to pass-through
template<class T> struct interval_type
{
	typedef typename std::remove_cv<T>::type type;
};

#define ZAIMONI_OVERRIDE_TYPE_STRUCT(TYPE,SRC,DEST)	\
template<>	\
struct TYPE<SRC>	\
{	\
	typedef DEST type;	\
};	\
	\
template<>	\
struct TYPE<const SRC>	\
{	\
	typedef DEST type;	\
};	\
	\
template<>	\
struct TYPE<volatile SRC>	\
{	\
	typedef DEST type;	\
};	\
	\
template<>	\
struct TYPE<const volatile SRC>	\
{	\
	typedef DEST type;	\
}

ZAIMONI_OVERRIDE_TYPE_STRUCT(interval_type,float,boost::numeric::interval<float>);
ZAIMONI_OVERRIDE_TYPE_STRUCT(interval_type,double,boost::numeric::interval<double>);
ZAIMONI_OVERRIDE_TYPE_STRUCT(interval_type,long double,boost::numeric::interval<long double>);

// don't undefine after migrating to Zaimoni.STL
#undef ZAIMONI_OVERRIDE_TYPE_STRUCT

// extend numerical error support to boost::numeric::interval
template<>
template<class T>
struct numerical<boost::numeric::interval<T> >
{
	enum {
		error_tracking = 1
	};
	typedef typename std::remove_cv<T>::type exact_type;
	typedef typename std::remove_cv<T>::type exact_arithmetic_type;

	static long double error(const boost::numeric::interval<T>& src) {
		boost::numeric::interval<long double> err(src.upper());
		err -= src.lower();
		return err.upper();
	}
	static bool causes_division_by_zero(const boost::numeric::interval<T>& src)
	{
		if (int_as<0,T>() > src.lower() && int_as<0,T>() < src.upper()) return true;
		if (int_as<0,T>() == src.lower() && int_as<0,T>() == src.upper()) return true;
		return false;	
	}
	static constexpr bool equals(const boost::numeric::interval<T>& lhs, exact_type rhs) {return lhs.lower()==rhs && lhs.upper()==rhs;};
};

// XXX anything using this class could be micro-optimized
template<class T>
struct fp_compare
{
	static bool good_sum_lt(const T& lhs, const T& rhs)
	{
		int exponent[2];
		// only has to work reasonably after preprocessing by rearrange sum
		frexp(lhs, exponent+0);
		frexp(rhs, exponent+1);
		return exponent[0]<exponent[1];
	}
};

template<>
template<class T>
struct fp_compare<boost::numeric::interval<T> >
{
	static bool good_sum_lt(const boost::numeric::interval<T>& lhs, const boost::numeric::interval<T>& rhs)
	{
		int exponent[5];
		// only has to work reasonably after preprocessing by rearrange sum
		frexp(lhs.lower(),exponent+0);
		frexp(lhs.upper(),exponent+1);
		frexp(rhs.lower(),exponent+2);
		frexp(rhs.upper(),exponent+3);
		exponent[4] = (exponent[0]<exponent[1] ? exponent[1] : exponent[0]);
		exponent[5] = (exponent[2]<exponent[3] ? exponent[3] : exponent[2]);
		return exponent[4]<exponent[5];
	}
};


// interval arithmetic wrappers
// we need proper function overloading here so use static member functions of a template class
template<class T>
struct lossy
{
	static typename interval_type<T>::type sum(typename const_param<T>::type lhs, typename const_param<T>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		typename interval_type<T>::type ret(lhs);
		ret += rhs;
		if (incoming_finite && !isFinite(ret)) throw std::overflow_error("addition");
		return ret;
	}
	static typename interval_type<T>::type sum(typename interval_type<T>::type lhs, typename const_param<T>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		lhs += rhs;
		if (incoming_finite && !isFinite(lhs)) throw std::overflow_error("addition");
		return lhs;
	}
	static typename interval_type<T>::type sum(typename const_param<T>::type lhs, typename interval_type<T>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		rhs += lhs;
		if (incoming_finite && !isFinite(rhs)) throw std::overflow_error("addition");
		return rhs;
	}
	static typename interval_type<T>::type sum(typename interval_type<T>::type lhs, typename const_param<typename interval_type<T>::type>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		lhs += rhs;
		if (incoming_finite && !isFinite(lhs)) throw std::overflow_error("addition");
		return lhs;
	}

	static typename interval_type<T>::type product(typename const_param<T>::type lhs, typename const_param<T>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		typename interval_type<T>::type ret(lhs);
		ret *= rhs;
		if (incoming_finite && !isFinite(ret)) throw std::overflow_error("product");
		return ret;
	}
	static typename interval_type<T>::type product(typename interval_type<T>::type lhs, typename const_param<T>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		lhs *= rhs;
		if (incoming_finite && !isFinite(lhs)) throw std::overflow_error("product");
		return lhs;
	}
	static typename interval_type<T>::type product(typename const_param<T>::type lhs, typename interval_type<T>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		rhs *= lhs;
		if (incoming_finite && !isFinite(rhs)) throw std::overflow_error("product");
		return rhs;
	}
	static typename interval_type<T>::type product(typename interval_type<T>::type lhs, typename const_param<typename interval_type<T>::type>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		lhs *= rhs;
		if (incoming_finite && !isFinite(lhs)) throw std::overflow_error("product");
		return lhs;
	}

	static typename interval_type<T>::type quotient(typename interval_type<T>::type lhs, typename const_param<T>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		if (causes_division_by_zero(rhs)) throw std::runtime_error("division by zero NaN " __FILE__ ":" DEEP_STRINGIZE(__LINE__));
		lhs /= rhs;
		if (incoming_finite && !isFinite(lhs)) throw std::overflow_error("quotient");
		return lhs;
	}

	static typename interval_type<T>::type quotient(typename interval_type<T>::type lhs, typename const_param<typename interval_type<T>::type>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		if (causes_division_by_zero(rhs)) throw std::runtime_error("division by zero NaN " __FILE__ ":" DEEP_STRINGIZE(__LINE__));
		const bool infinite_out_ok = (0.0==rhs.lower() || 0.0==rhs.upper());
		lhs /= rhs;
		if (incoming_finite && !infinite_out_ok && !isFinite(lhs)) throw std::overflow_error("quotient");
		return lhs;
	}

	static typename interval_type<T>::type self_product(typename interval_type<T>::type& lhs, typename const_param<T>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		lhs *= rhs;
		if (incoming_finite && !isFinite(lhs)) throw std::overflow_error("product");
		return lhs;
	}

	static typename interval_type<T>::type self_product(typename interval_type<T>::type& lhs, typename const_param<typename interval_type<T>::type>::type rhs) 
	{
		const bool incoming_finite = isFinite(lhs) && isFinite(rhs);
		lhs *= rhs;
		if (incoming_finite && !isFinite(lhs)) throw std::overflow_error("product");
		return lhs;
	}

	static typename interval_type<T>::type square(typename const_param<T>::type x) 
	{
		const bool incoming_finite = isFinite(x);
		typename interval_type<T>::type ret(square(x));
		if (incoming_finite && !isFinite(ret)) throw std::overflow_error("square");
		return ret;
	}

	static typename interval_type<T>::type square(typename interval_type<T>::type x) 
	{
		const bool incoming_finite = isFinite(x);
		x = square(x);;
		if (incoming_finite && !isFinite(x)) throw std::overflow_error("square");
		return x;
	}
};

template<class T>
boost::numeric::interval<T> quotient(const boost::numeric::interval<T>& lhs, const boost::numeric::interval<T>& rhs)
{
	return lossy<typename std::remove_cv<T>::type>::quotient(lhs,rhs);
}

}	// namespace math
}	// namespace zaimoni

#endif
