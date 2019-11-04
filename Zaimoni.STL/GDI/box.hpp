#ifndef ZAIMONI_STL_GDI_BOX_HPP
#define ZAIMONI_STL_GDI_BOX_HPP 1

/* (C)2019 Kenneth Boyd, license: MIT.txt */

#include <functional>
#ifdef IRRATIONAL_CAUTION
#include "../Logging.h"
#endif

namespace zaimoni {
namespace gdi {

template<class T>	// T assumed to be "like a vector space"
class box
{
private:
	T _top_left;		// nonstrict bounds
	T _bottom_right;
public:
	box() = default;
	box(const T& tl, const T& br) : _top_left(tl), _bottom_right(br) {};
	box(const box& src) = default;
	~box() = default;
	box& operator=(const box& src) = default;

	T& tl_c() { return _top_left; }
	T& br_c() { return _bottom_right; }
	const T& tl_c() const { return _top_left; }
	const T& br_c() const { return _bottom_right; }

	bool contains(const T& src) const {
		static std::less_equal<typename T::coord_type> lte;
		return pointwise_test(_top_left, src, lte) && pointwise_test(src, _bottom_right, lte);
	}

	box& operator+=(const T& rhs) {
		_top_left += rhs;
		_bottom_right += rhs;
		return *this;
	}

	box& operator-=(const T& rhs) {
		_top_left -= rhs;
		_bottom_right -= rhs;
		return *this;
	}

	box& operator*=(const typename T::coord_type& rhs) {
		_top_left *= rhs;
		_bottom_right *= rhs;
		return *this;
	}

	box& operator/=(const typename T::coord_type& rhs) {
		_top_left /= rhs;
		_bottom_right /= rhs;
		return *this;
	}
};

// enumerate coordinates "on edge of box" is practical for integer coordinates, but viable canonical schemas are dimension-dependent
// prototyped in Rogue Survivor Revived, relicensed for here
template<class T>
T Linf_border_sweep(const int radius, unsigned int i, const int x0, const int y0)	// "Lesbegue infinity border sweep"
{
	if (i == 0) return T(x0 - radius, y0 - radius);
#ifdef IRRATIONAL_CAUTION
#define Z_UINT_MAX ((unsigned int)(-1)/2)
	assert(1 <= radius);
	assert(Z_UINT_MAX/8 >= radius);
	assert(8 * radius > i);
#undef Z_INT_MAX
#endif
	const int radius2 = 2 * radius;
	if (i < radius2) return T(x0 - radius + i, y0 - radius);
	i -= radius2;
	if (i < radius2) return T(x0 + radius, y0 - radius + i);
	i -= radius2;
	if (i < radius2) return T(x0 + radius - i, y0 + radius);
	i -= radius2;
	/* if (i < radius2) */ return T(x0 - radius, y0 + radius - i);
}

}	// namespace gdi
}	// namespace zaimoni

#endif