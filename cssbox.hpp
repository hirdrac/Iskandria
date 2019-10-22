#ifndef CSSBOX_HPP
#define CSSBOX_HPP 1

#include <utility>
#include <memory>
#include <vector>

namespace css {

class box {
#undef LEFT
#undef TOP
#undef RIGHT
#undef BOTTOM
#undef WIDTH
#undef HEIGHT
#undef REFLOW
public:
	enum auto_legal {
		LEFT = 0,
		TOP,
		RIGHT,
		BOTTOM,	// highest index for margin, padding
		WIDTH,
		HEIGHT,	// highest index for auto flagging
	};
protected:
	enum {
		REFLOW = HEIGHT + 1
	};

	unsigned char _auto;	// bitmap: margins, width, height
	unsigned char _auto_recalc;	// margin or height/width
	std::pair<int, int> _origin;	// (x,y) i.e. (left,top): relative to container
	std::pair<int, int> _screen;	// (x,y) i.e. (left,top): global
private:
	std::pair<int, int> _size;
	std::pair<int, int> _size_min;
	std::pair<int, int> _size_max;
	int _margin[4];
	int _padding[4];
	std::weak_ptr<box> _parent;
	std::shared_ptr<box> _self;
protected:
	box(bool bootstrap = false);
	box(const box& src) = default;
	box(box&& src) = default;
	box& operator=(const box& src) = default;
	box& operator=(box&& src) = default;
public:
	virtual ~box() = default;

	// auto values
	bool is_auto(auto_legal src) const { return _auto & (1ULL << src); }
	void set_auto(auto_legal src);

	// width/height
	auto size() const { return _size; }
	int width() const { return _size.first; }
	int height() const { return _size.second; }
	int min_width() const { return _size_min.first; }
	int min_height() const { return _size_min.second; }
	int max_width() const { return _size_max.first; }
	int max_height() const { return _size_max.second; }

	void width(int w);
	void height(int h);
	void min_width(int w);
	void min_height(int h);
	void max_width(int w);
	void max_height(int h);

	// padding, margin
	template<int src> int padding() const {
		static_assert(0 <= src && WIDTH > src, "0 <= src && WIDTH > src");
		return _padding[src];
	}

	template<int src> int margin() const {
		static_assert(0 <= src && WIDTH > src, "0 <= src && WIDTH > src");
		return _margin[src];
	}
	template<int src> void _set_margin(int x) {
		static_assert(0 <= src && WIDTH > src, "0 <= src && WIDTH > src");
		_auto_recalc &= ~(1ULL << src);
		_margin[src] = x;
	}
	template<int src> void set_margin(int x) {
		static_assert(0 <= src && WIDTH > src, "0 <= src && WIDTH > src");
		_auto &= ~(1ULL << src);
		_set_margin<src>(x);
	}

	int full_width() const { return width() + padding<LEFT>() + padding<RIGHT>(); }
	int full_height() const { return height() + padding<TOP>() + padding<BOTTOM>(); }

	// layout recalculation (unsure about access control here)
	std::shared_ptr<box> parent() const { return _parent.lock(); }
	auto origin() const { return _origin; }

	virtual bool flush();
	virtual int need_recalc() const;	// return value is C error code convention; 0 no-action, negative error, positive action code
	void recalc();

	virtual void draw() const;
	virtual void set_origin(std::pair<int, int> logical_origin);
	virtual void screen_coords(std::pair<int, int> logical_origin);

	void horizontal_centering(int ub, std::pair<int, int> origin);
	void vertical_centering(int ub, std::pair<int, int> origin);
protected:
	virtual void schedule_reflow();
	void set_parent(std::shared_ptr<box>& src);
	void _width(int w);
	void _height(int h);

private:
	virtual void _recalc(int code);
};

class box_dynamic : public box
{
private:
	std::vector<std::shared_ptr<box> > _contents;
public:
	// have to allow public default-construction because the top-level box isn't tied to content
	box_dynamic(bool bootstrap = false) : box(bootstrap) {}
	box_dynamic(const box_dynamic& src) = default;
	box_dynamic(box_dynamic&& src) = default;
	box_dynamic& operator=(const box_dynamic& src) = default;
	box_dynamic& operator=(box_dynamic&& src) = default;
	~box_dynamic() = default;

	// content management
	void append(std::shared_ptr<box> src);

	virtual void draw() const;
	virtual void screen_coords(std::pair<int, int> logical_origin);
protected:
	virtual void schedule_reflow();
private:
	virtual bool flush();
	virtual int need_recalc() const;
	virtual void _recalc(int code);
};

}	// namespace css

#endif
