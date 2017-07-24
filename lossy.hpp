// lossy.hpp

#ifndef LOSSY_HPP
#define LOSSY_HPP 1

#include <cmath>
#include <boost/numeric/interval.hpp>

#include "Zaimoni.STL/Logging.h"
#include "Zaimoni.STL/augment.STL/type_traits"

namespace zaimoni {

using std::fpclassify;
using std::isfinite;
using std::isinf;
using std::isnan;
using std::signbit;

namespace math {

using std::fpclassify;
using std::isfinite;
using std::isinf;
using std::isnan;
using std::signbit;

template<class T>
constexpr bool isnan(const boost::numeric::interval<T>& x)
{
	return empty(x)	// null set isn't that useful
        || isnan(x.lower()) || isnan(x.upper()) 	// intuitive
		|| (isinf(x.lower()) && isinf(x.upper()) && x.lower()<x.upper());	// also disallow (-infinity,infinity) (total loss of information)
}

template<class T>
constexpr bool isfinite(const boost::numeric::interval<T>& x)
{
	return std::isfinite(x.lower()) && std::isfinite(x.upper());
}

// several choices of how to define.
template<class T>
constexpr bool signbit(const boost::numeric::interval<T>& x)
{
	return std::signbit(x.upper());
}

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

// interval arithmetic wrappers
// we need proper function overloading here so use static member functions of a template class
template<class T>
struct lossy
{
	static typename interval_type<T>::type sum(typename const_param<T>::type lhs, typename const_param<T>::type rhs) 
	{
		const bool incoming_finite = isfinite(lhs) && isfinite(rhs);
		typename interval_type<T>::type ret(lhs);
		ret += rhs;
		if (incoming_finite && !isfinite(ret)) throw std::overflow_error("addition");
		return ret;
	}
	static typename interval_type<T>::type sum(typename interval_type<T>::type lhs, typename const_param<T>::type rhs) 
	{
		const bool incoming_finite = isfinite(lhs) && isfinite(rhs);
		lhs += rhs;
		if (incoming_finite && !isfinite(lhs)) throw std::overflow_error("addition");
		return lhs;
	}
	static typename interval_type<T>::type sum(typename const_param<T>::type lhs, typename interval_type<T>::type rhs) 
	{
		const bool incoming_finite = isfinite(lhs) && isfinite(rhs);
		rhs += lhs;
		if (incoming_finite && !isfinite(rhs)) throw std::overflow_error("addition");
		return rhs;
	}
	static typename interval_type<T>::type sum(typename interval_type<T>::type lhs, typename const_param<typename interval_type<T>::type>::type rhs) 
	{
		const bool incoming_finite = isfinite(lhs) && isfinite(rhs);
		lhs += rhs;
		if (incoming_finite && !isfinite(lhs)) throw std::overflow_error("addition");
		return lhs;
	}

	static typename interval_type<T>::type product(typename const_param<T>::type lhs, typename const_param<T>::type rhs) 
	{
		const bool incoming_finite = isfinite(lhs) && isfinite(rhs);
		typename interval_type<T>::type ret(lhs);
		ret *= rhs;
		if (incoming_finite && !isfinite(ret)) throw std::overflow_error("product");
		return ret;
	}
	static typename interval_type<T>::type product(typename interval_type<T>::type lhs, typename const_param<T>::type rhs) 
	{
		const bool incoming_finite = isfinite(lhs) && isfinite(rhs);
		lhs *= rhs;
		if (incoming_finite && !isfinite(lhs)) throw std::overflow_error("product");
		return lhs;
	}
	static typename interval_type<T>::type product(typename const_param<T>::type lhs, typename interval_type<T>::type rhs) 
	{
		const bool incoming_finite = isfinite(lhs) && isfinite(rhs);
		rhs *= lhs;
		if (incoming_finite && !isfinite(rhs)) throw std::overflow_error("product");
		return rhs;
	}
	static typename interval_type<T>::type product(typename interval_type<T>::type lhs, typename const_param<typename interval_type<T>::type>::type rhs) 
	{
		const bool incoming_finite = isfinite(lhs) && isfinite(rhs);
		lhs *= rhs;
		if (incoming_finite && !isfinite(lhs)) throw std::overflow_error("product");
		return lhs;
	}

	static typename interval_type<T>::type self_product(typename interval_type<T>::type& lhs, typename const_param<T>::type rhs) 
	{
		const bool incoming_finite = isfinite(lhs) && isfinite(rhs);
		lhs *= rhs;
		if (incoming_finite && !isfinite(lhs)) throw std::overflow_error("product");
		return lhs;
	}

	static typename interval_type<T>::type self_product(typename interval_type<T>::type& lhs, typename const_param<typename interval_type<T>::type>::type rhs) 
	{
		const bool incoming_finite = isfinite(lhs) && isfinite(rhs);
		lhs *= rhs;
		if (incoming_finite && !isfinite(lhs)) throw std::overflow_error("product");
		return lhs;
	}

	static typename interval_type<T>::type square(typename const_param<T>::type x) 
	{
		const bool incoming_finite = isfinite(x);
		typename interval_type<T>::type ret(square(x));
		if (incoming_finite && !isfinite(ret)) throw std::overflow_error("square");
		return ret;
	}

	static typename interval_type<T>::type square(typename interval_type<T>::type x) 
	{
		const bool incoming_finite = isfinite(x);
		x = square(x);;
		if (incoming_finite && !isfinite(x)) throw std::overflow_error("square");
		return x;
	}
};

}	// namespace math
}	// namespace zaimoni

#ifdef TEST_APP
// example build line (have to copy from *.hpp to *.cpp or else main not seen
// g++ -olossy.exe -Llib/host -DTEST_APP lossy.cpp -lz_stdio_log
// If doing INFORM-based debugging
// g++ -std=c++11 -olossy.exe -DTEST_APP lossy.cpp -Llib/host.isk -lz_log_adapter -lz_stdio_log -lz_format_util

#include <stdlib.h>
#include <stdio.h>

// console-mode application
#define STRING_LITERAL_TO_STDOUT(A) fwrite(A,sizeof(A)-1,1,stdout)
#define C_STRING_TO_STDOUT(A) fwrite(A,strlen(A),1,stdout)
#define STL_STRING_TO_STDOUT(A) fwrite((A).data(),(A).size(),1,stdout)
#define STL_PTR_STRING_TO_STDOUT(A) fwrite((A)->data(),(A)->size(),1,stdout)

#define INTERVAL_TO_STDOUT(A,UNIT)	\
	if (A.lower()==A.upper()) {	\
		sprintf(buf,"%.16g",A.lower());	\
		C_STRING_TO_STDOUT(buf);	\
		STRING_LITERAL_TO_STDOUT(UNIT);	\
	} else {	\
		STRING_LITERAL_TO_STDOUT("[");	\
		sprintf(buf,"%.16g",A.lower());	\
		C_STRING_TO_STDOUT(buf);	\
		STRING_LITERAL_TO_STDOUT(", ");	\
		sprintf(buf,"%.16g",A.upper());	\
		C_STRING_TO_STDOUT(buf);	\
		STRING_LITERAL_TO_STDOUT("]");	\
		STRING_LITERAL_TO_STDOUT(UNIT);	\
	}

int main(int argc, char* argv[])
{	// parse options
	char buf[100];

	STRING_LITERAL_TO_STDOUT("starting main\n");

	double lhs = 2.0;
	double rhs = 2.0;
	boost::numeric::interval<double> lhs_2 = 2.0;
	boost::numeric::interval<double> rhs_2 = 2.0;

	zaimoni::math::lossy<double>::sum(lhs,rhs);
	zaimoni::math::lossy<double>::product(lhs,rhs);

	zaimoni::math::lossy<double>::sum(lhs,rhs_2);
	zaimoni::math::lossy<double>::product(lhs,rhs_2);
	
	zaimoni::math::lossy<double>::sum(lhs_2,rhs);
	zaimoni::math::lossy<double>::product(lhs_2,rhs);

	zaimoni::math::lossy<double>::sum(lhs_2,rhs_2);
	zaimoni::math::lossy<double>::product(lhs_2,rhs_2);

	STRING_LITERAL_TO_STDOUT("tests finished\n");
}

#endif

#endif