// iterator_array_size.hpp

#ifndef ITERATOR_ARRAY_SIZE_HPP
#define ITERATOR_ARRAY_SIZE_HPP 1

#include "Logging.h"
#include "iterator.on.hpp"
#include <iterator>

namespace zaimoni {

template<class T>
class iterator_array_size
{
private:
	T* _src;
	size_t _i;

	bool can_dereference() {return _src && _i < _src.size();}
public:
	typedef typename T::difference_type difference_type;	// these four aligned with container
	typedef typename T::value_type value_type;
	typedef typename T::reference reference;
	typedef typename T::pointer pointer;
	typedef std::random_access_iterator_tag iterator_category;

	bool is_valid() {return _src && _i <= _src.size();}	// for post-condition testing

	iterator_array_size(T* src = 0, size_t offset = 0) : _src(src),_i(offset) {};
	iterator_array_size(const iterator_array_size& src) = default;
	iterator_array_size(iterator_array_size&& src) = default;
	~iterator_array_size() = default;
	iterator_array_size& operator=(const iterator_array_size& src) = default;
	iterator_array_size& operator=(iterator_array_size&& src) = default;

	bool operator==(const iterator_array_size& rhs) const {return _src==rhs._src && _i==rhs._i;}

    reference operator*() const {
		assert(can_dereference());
		return (*_src)[_i]; 
	};
    iterator_array_size& operator++() {	// prefix
		if (_src->size() > _i) _i++;
		return *this;
	};
    iterator_array_size operator++(int) {	// postfix
		iterator_array_size ret(*this);
		if (_src->size() > _i) _i++;
		return ret;
	};

	// bidirectional
    iterator_array_size& operator--() {	// prefix
		if (0 < _i) _i--;
		return *this;
	};
    iterator_array_size operator--(int) {	// postfix
		iterator_array_size ret(*this);
		if (0 < _i) _i--;
		return ret;
	};

	// random access
    iterator_array_size& operator+=(difference_type n) {
		if (0<n) {
			assert(_src.size()-_i >= n);
			_i += n;
		} else if (0>n) {
			assert(_i >= -n);
			_i += n;
		}
		return *this;
	};

    iterator_array_size& operator-=(difference_type n) {
		if (0<n) {
			assert(_i >= -n);
			_i -= n;
		} else if (0>n) {
			assert(_src.size()-_i >= n);
			_i -= n;
		}
		return *this;
	};
	difference_type operator-(const iterator_array_size& rhs) {
		assert(_src==rhs._src);
		return _i-rhs._i;
	}

    reference operator[](size_t n) const {
		assert(_src->size()-_i >= n);
		return _src[_i+n];
	}

    bool operator<(const iterator_array_size& rhs) const {
		assert(_src==rhs._src);
		return _i < rhs._i;
	}

	ZAIMONI_ITER_DECLARE(iterator_array_size)
};

template<class T>
class const_iterator_array_size
{
private:
	T* _src;
	size_t _i;

	bool can_dereference() {return _src && _i < _src.size();}
public:
	typedef typename T::difference_type difference_type;	// these four aligned with container
	typedef const typename T::value_type value_type;
	typedef const typename T::reference reference;
	typedef const typename T::pointer pointer;
	typedef std::random_access_iterator_tag iterator_category;

	bool is_valid() {return _src && _i <= _src.size();}	// for post-condition testing

	const_iterator_array_size(T* src = 0, size_t offset = 0) : _src(src),_i(offset) {};
	const_iterator_array_size(const const_iterator_array_size& src) = default;
	const_iterator_array_size(const_iterator_array_size&& src) = default;
	~const_iterator_array_size() = default;
	const_iterator_array_size& operator=(const const_iterator_array_size& src) = default;
	const_iterator_array_size& operator=(const_iterator_array_size&& src) = default;

	bool operator==(const const_iterator_array_size& rhs) const {return _src==rhs._src && _i==rhs._i;}

    reference operator*() const {
		assert(can_dereference());
		return (*_src)[_i]; 
	};
    const_iterator_array_size& operator++() {	// prefix
		if (_src->size() > _i) _i++;
		assert(is_valid());
		return *this;
	};
    const_iterator_array_size operator++(int) {	// postfix
		const_iterator_array_size ret(*this);
		if (_src->size() > _i) _i++;
		assert(is_valid());
		return ret;
	};

	// bidirectional
    const_iterator_array_size& operator--() {	// prefix
		if (0 < _i) _i--;
		assert(is_valid());
		return *this;
	};
    const_iterator_array_size operator--(int) {	// postfix
		const_iterator_array_size ret(*this);
		if (0 < _i) _i--;
		assert(is_valid());
		return ret;
	};

    const_iterator_array_size& operator+=(difference_type n) {	// postfix
		if (0<n) {
			assert(_src.size()-_i >= n);
			_i += n;
		} else if (0>n) {
			assert(_i >= -n);
			_i += n;
		}
		assert(is_valid());
		return *this;
	};

    const_iterator_array_size& operator-=(difference_type n) {
		if (0<n) {
			assert(_i >= -n);
			_i -= n;
		} else if (0>n) {
			assert(_src.size()-_i >= n);
			_i -= n;
		}
		assert(is_valid());
		return *this;
	};
	difference_type operator-(const const_iterator_array_size& rhs) const {
		assert(_src==rhs._src);
		return _i-rhs._i;
	}

    reference operator[](size_t n) const { // random access
		assert(_src->size()-_i >= n);
		return _src[_i+n];
	}

    bool operator<(const const_iterator_array_size& rhs) const {
		assert(_src==rhs._src);
		return _i < rhs._i;
	}

	ZAIMONI_ITER_DECLARE(const_iterator_array_size)
};

}	// namespace zaimoni

#undef ZAIMONI_ITER_DECLARE

#endif


