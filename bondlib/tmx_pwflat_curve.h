// tmx_pwflat_curve.h - piecewise flat curve value type
#pragma once
#include <vector>
#include "ensure.h"
#include "tmx_pwflat.h"

namespace tmx::pwflat {

	// Pwflat curve value type.
	template<class T = double, class F = double>
	class curve {
		std::vector<T> t;
		std::vector<F> f;
		F _f;
		size_t off;
	public:
		// constant curve
		curve(F _f = NaN<F>)
			: _f{ _f }
		{ }
		curve(size_t n, const T* t_, const F* f_, F _f = NaN<F>)
			: t(t_, t_ + n), f(f_, f_ + n), _f(_f), off(0)
		{
			ensure(ok());
		}
		curve(const std::span<T>& t, const std::span<F>& f, F _f = NaN<F>)
			: curve(t.size(), t.data(), f.data(), _f)
		{
			ensure(t.size() == f.size());
			ensure(ok());
		}
		curve(const curve&) = default;
		curve& operator=(const curve&) = default;
		curve(curve&&) = default;
		curve& operator=(curve&&) = default;
		~curve()
		{ }

		bool operator==(const curve& c) const
		{
			return off == c.offset()
				and std::equal(t.data() + off, t.data() + t.size(), c.time())
				and std::equal(f.data() + off, f.data() + f.size(), c.rate())
				and (_f == c.extrapolate() or std::isnan(_f) and std::isnan(c.extrapolate()));
		}
		bool operator!=(const curve& c) const
		{
			return !operator==(c);
		}

		bool ok() const
		{
			return t.size() == f.size() and (t.size() == 0 or t[0] > 0 and monotonic(t.begin(), t.end()));
		}

		size_t offset() const
		{
			return off;
		}
		size_t size() const
		{
			return t.size() - off;
		}
		const T* time() const
		{
			return t.data() + off;
		}
		const F* forward() const
		{
			return f.data() + off;
		}
		const F* rate() const
		{
			return forward();
		}
		std::pair<T, F> back() const
		{
			ensure(0 != size());

			return { t.back(), f.back() };
		}

		// add point
		curve& extend(T t_, F f_)
		{
			ensure(size() == 0 || t_ > t.back());

			t.push_back(t_);
			f.push_back(f_);

			return *this;
		}
		// Get extrapolated value.
		F extrapolate() const
		{
			return _f;
		}
		// Set extrapolated value.
		curve& extrapolate(F f_)
		{
			_f = f_;
		}

		// Parallel shift
		curve& shift(F df)
		{
			std::for_each(f.begin(), f.end(), [df](F fi) { fi += df; });
			_f += df;

			return *this;
		}

		// Forward at time u
		curve& translate(T u)
		{
			auto tu = pwflat::translate(u, std::span(t));
			off = t.size() - tu.size();

			return *this;
		}

		F value(T u) const
		{
			return pwflat::value(u, size(), time(), rate(), _f);
		}
		F operator()(T u) const
		{
			return value(u);
		}

		F integral(T u) const
		{
			return pwflat::integral(u, size(), time(), rate(), _f);
		}
		F discount(T u) const
		{
			return pwflat::discount(u, size(), time(), rate(), _f);
		}
		F spot(T u) const
		{
			return pwflat::spot(u, size(), time(), rate(), _f);
		}
	};

#ifdef _DEBUG

	template<class X>
	inline int pwflat_curve_test()
	{
		X t[] = { 1, 2, 3 };
		X f[] = { .1, .2, .3 };

		{
			curve c(.1);
			ensure(0 == c.size());
			ensure(.1 == c(.2));
			ensure(c.extrapolate() == c(.2));
			curve c2{ c };
			ensure(c2 == c);
			c = c2;
			ensure(!(c != c2));
		}
		{
			curve c(3, t, f);
			
			ensure(3 == c.size());
			ensure(.1 == c.value(1));
			ensure(std::isnan(c.extrapolate()));
			curve c2{ c };
			ensure(c2 == c);
			c = c2;
			ensure(!(c != c2));			

			ensure(std::isnan(c(-1)));
			for (size_t i = 0; i < 3; ++i) {
				ensure(f[i] == c(t[i]));
			}
			ensure(f[0] == c(0.5));
			ensure(f[1] == c(1.5));
			ensure(f[2] == c(2.5));
			ensure(std::isnan(c(3.5)));
		}
		{
			curve c(3, t, f);
			c.translate(1);
		}

		return 0;
	}

#endif // _DEBUG

}