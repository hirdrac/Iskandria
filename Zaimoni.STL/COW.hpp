#ifndef ZAIMONI_STL_COW_HPP
#define ZAIMONI_STL_COW_HPP 1

#include <memory>
#include <variant>
#include <optional>

// copy-on-write -- prioritizing RAM over speed
namespace zaimoni {

template<class T>
class COW final {
	std::shared_ptr<const T> _read; // std::variant here would break operator=
	std::unique_ptr<T> _write;

public:
	COW() = default;
	COW(const COW& src) = delete;
	COW(COW&& src) = default;
	COW(const std::shared_ptr<const T>& src) noexcept : _read(src) {};
	COW(std::unique_ptr<T>&& src)  noexcept : _write(std::move(src)) {};
	~COW() = default;

	COW(COW& src) : _read(src._read) {
		if (src._write) {
			_read = std::shared_ptr<const T>(src._write.release());
			src = _read;
		}
	}

	COW& operator=(const COW& src) = delete;
	COW& operator=(COW&& src) = default;

	COW& operator=(const std::shared_ptr<const T>& src) noexcept {
		_read = src;
		_write.reset();
		return *this;
	}

	COW& operator=(std::unique_ptr<T>&& src) noexcept {
		_write = std::move(src);
		_read.reset();
		return *this;
	}

	COW& operator=(COW& src) noexcept {
		if (this == &src) return *this;
		if (src._read) return *this = _read;
		if (!src._write) {
			_read.reset();
			_write.reset();
			return *this;
		}
		_read = std::shared_ptr<const T>(src._write.release());
		src = _read;
		return *this;
	}

	const T* get_c() const {
		if (_read) return _read.get();
		if (_write) return _write.get();
		return nullptr;
	}

	const T* get() const { return get_c(); }

	T* get() { // multi-threaded: race condition against operoator=(COW& src)
		if (_read) _rw_clone();
		if (_write) return _write.get();
		return nullptr;
	}

	/// <returns>std::nullopt, or .second is non-null and .first is non-null if non-const operations are not logic errors</returns>
	template<class U> std::optional<std::pair<U*, const U*> > get_rw() {
		if (auto r = _get_rw<U>()) {
			std::pair<U*, const U*> ret;
			auto pre_exec = std::get_if<U*>(&(*r));
			ret.second = ret.first = pre_exec ? *pre_exec : nullptr;
			if (!ret.second) {
				if (auto pre_test = std::get_if<const U*>(&(*r))) ret.second = *pre_test;
				else return std::nullopt;
			}
			return ret;
		}
		return std::nullopt;
	}

/*
	operator const T* () const { return get_c(); }
	operator T* () { return get(); }
*/
private:
	void _rw_clone() requires requires() { _read->clone(); } {
		_write = std::unique_ptr<T>(_read->clone());
		_read.reset();
	}

	template<class U> std::optional<std::variant<U*, const U*> > _get_rw() {
		if (_read) return dynamic_cast<const U*>(_read.get());
		if (_write) return dynamic_cast<U*>(_write.get());
		return std::nullopt;
	}
};

}

#endif