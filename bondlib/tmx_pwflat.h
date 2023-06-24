// tmx_pwflat.h - left continuous piecewise flat curve
/*
		   { f[i] if t[i-1] < t <= t[i];
	f(t) = { _f   if t > t[n-1];
		   { NaN  if t < 0
		   { f[0] if t = 0
	F                                   _f
	|        f[1]             f[n-1] o--------
	| f[0] o------          o--------x
	x------x      ... ------x
	|
	0-----t[0]--- ... ---t[n-2]---t[n-1]--- T
*/
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <cmath>
#include <algorithm>
#include "ensure.h"
#include "tmx_view.h"

namespace tmx::pwflat {

	// strictly increasing values
	template<class I>
	constexpr bool monotonic(const I b, const I e)
	{
		using T = typename std::iterator_traits<I>::value_type;

		// std::is_sorted(b, e, less) is not correct 
		return e == std::adjacent_find(b, e, std::greater_equal<T>{});
	}
	template<class T>
	constexpr bool monotonic(size_t n, const T* t)
	{
		return monotonic(t, t + n);
	}
	template<class T>
	constexpr bool monotonic(const view<T>& t)
	{
		return monotonic(t.begin(), t.end());
	}
	template<class T>
	constexpr bool monotonic(const std::initializer_list<T>& t)
	{
		return monotonic(t.begin(), t.end());
	}

#ifdef _DEBUG

	inline int monotonic_test()
	{
		static_assert(monotonic<int>(0, nullptr));
		static_assert(monotonic({ 1 }));
		static_assert(monotonic({ 1,2 }));
		static_assert(!monotonic({ 1.,1. }));

		return 0;
	}

#endif // _DEBUG

	// NVI base class for piecewise flat curve.
	template<class T = double, class F = double>
	struct curve {
		virtual ~curve() = default;
		// f(t) is the forward rate at time t
		F forward(T u) const
		{
			return _forward(u);
		}
		F operator()(T u) const
		{
			return _forward(u);
		}
		// D_t(u) is the price at time t of a zero coupon bond maturing at time u.
		// D_t(u) = exp(-int_t^u f(s) ds).
		F discount(T u, T t = 0) const
		{
			return _discount(u, t);
		}
		// r_t(u) is the spot rate at time t over the interval [t, u].
		// D_t(u) = exp(-(u - t) r_t(u)).
		F spot(T u, T t = 0) const
		{
			return _spot(u, t);
		}
	private:
		virtual F _forward(T u) const = 0;
		virtual F _discount(T u, T t) const = 0;
		virtual F _spot(T u, T t) const = 0;
	};

	// Non-owning view of piecewise flat curve.
	template<class T = double, class F = double>
	struct curve_view : public curve<T, F> {
		view<T> t;
		view<F> f;
		F _f; // extrapolation value

		// constant curve
		curve_view(F _f = NaN<F>)
			: t{}, f{}, _f(_f)
		{ }
		curve_view(view<T> t, view<F> f, F _f = NaN<F>)
			: t(t), f(f), _f(_f)
		{
			ensure(t.size() == f.size());
			ensure(t.size() == 0 or t[0] > 0);
			ensure(monotonic(t));
		}
		curve_view(size_t n, const T* t, const F* f, F _f = NaN<F>)
			: curve_view(view<T>(n, t), view<F>(n, f), _f)
		{ }
		curve_view(const curve_view&) = default;
		curve_view& operator=(const curve_view&) = default;
		curve_view(curve_view&&) = default;
		curve_view& operator=(curve_view&&) = default;
		virtual ~curve_view() = default;

		bool operator==(const curve_view& cv) const
		{
			return size() == cv.size()
				&& t == cv.t
				&& f == cv.f
				&& (std::isnan(extrapolate()) and std::isnan(cv.extrapolate())
					or extrapolate() == cv.extrapolate());
		}

		size_t size() const
		{
			return t.size();
		}

		F extrapolate() const
		{
			return _f;
		}
		curve_view<T, F>& extrapolate(F f_)
		{
			_f = f_;

			return *this;
		}
		F _forward(T u) const override
		{
			if (u < 0)
				return NaN<F>;
			if (size() == 0)
				return extrapolate();

			auto ti = std::lower_bound(t.begin(), t.end(), u);

			return ti == t.end() ? _f : f[ti - t.begin()];
		}

		// int_t^u f(t) dt
		F integral(T u, T t0 = 0) const
		{
			if (u < t0)
				return NaN<F>;
			if (u == t0)
				return 0;
			if (size() == 0)
				return (u - t0) * extrapolate();

			F I = 0;
			T t_ = t0;

			size_t i = t.offset(t0);
			for (; i < size() and t[i] <= u; ++i) {
				I += f[i] * (t[i] - t_);
				t_ = t[i];
			}
			if (u > t_) {
				I += (i == size() ? extrapolate() : f[i]) * (u - t_);
			}

			return I;
		}
		// discount D_t(u) = exp(-int_t^u f(s) ds)
		// std::exp not constexpr
		F _discount(T u, T t0 = 0) const override
		{
			return std::exp(-integral(u, t0));
		}

		// D_t(u) = exp(-(u - t) r_t(u))
		// r_t(u) = \int_t^u f(s) ds / (u - t)
		// r_t(u) = f(u) if u <= t[offset]
		constexpr F _spot(T u, T t0 = 0) const override
		{
			size_t i = t.offset(u);

			return u < t0 ? NaN<F>
				: i == size() ? extrapolate()
				: u <= t[i] ? f[i]
				: integral(u, t0) / (u - t0);
		}

#ifdef _DEBUG
		static int test()
		{
			T t[] = { 1, 2, 3 };
			F f[] = { 2, 3, 4 };
			auto cv = curve_view(view(t), view(f));
			{
				{
					double v = cv(-0.1);
					assert(v != v);
				}
				assert(2 == cv(0.));
				assert(2 == cv(0.1));
				assert(2 == cv(1.));
				assert(3 == cv(1.1));
				assert(4 == cv(2.9));
				assert(4 == cv(3.));
				{
					double v = cv(3.1);
					assert(v != v);
				}
			}
			{
				{
					double v = cv.integral(-0.1);
					assert(v != v);
				}
				assert(0 == cv.integral(0.));
				assert(1 == cv.integral(0.5));
				assert(2 == cv.integral(1.));
				assert(2 + 3. / 2 == cv.integral(1.5));
				assert(5 == cv.integral(2.));
				assert(9 == cv.integral(3.));
				cv.extrapolate(5);
				assert(9 + 2.5 == cv.integral(3.5));
			}


			return 0;
		}
#endif // _DEBUG

	};

	// Pwflat curve value type.
	template<class T = double, class F = double>
	class curve_value : public curve_view<T, F> {
		std::vector<T> t;
		std::vector<F> f;
		void update()
		{
			curve_view<T, F>::t = view<T>(t.begin(), t.end());
			curve_view<T, F>::f = view<F>(f.begin(), f.end());
		}
	public:
		// constant curve
		constexpr curve_value(F _f = NaN<F>)
			: curve_view<T, F>(_f)
		{ }
		curve_value(size_t n, const T* t_, const F* f_, F _f = NaN<F>)
			: curve_view<T, F>(_f), t(t_, t_ + n), f(f_, f_ + n)
		{
			update();
		}
		// Promote view to a value.
		curve_value(const curve_view<T, F>& c)
			: curve_value(c.size(), c.time(), c.rate(), c.extrapolate())
		{ }
		curve_value(const curve_value&) = default;
		curve_value& operator=(const curve_value&) = default;
		curve_value(curve_value&&) = default;
		curve_value& operator=(curve_value&&) = default;
		~curve_value()
		{ }

		// add point
		curve_value& extend(T t_, F f_)
		{
			ensure(t.size() == 0 || t_ > t.back());

			t.push_back(t_);
			f.push_back(f_);
			update();

			return *this;
		}
		curve_value& push_back(const std::pair<T, F>& tf)
		{
			return extend(tf.first, tf.second);
		}
#ifdef _DEBUG

		static int test()
		{
			T t[] = { 1, 2, 3 };
			F f[] = { .1, .2, .3 };
			{
				curve_value c(.1);
				ensure(0 == c.size());
				ensure(.1 == c.extrapolate());
				curve_value c2{ c };
				ensure(c2 == c);
				c = c2;
				ensure(!(c != c2));
				ensure(.1 == c(.2));
			}
			{
				curve_value c(3, t, f);

				ensure(3 == c.size());
				ensure(f[0] == c(1.));
				ensure(std::isnan(c.extrapolate()));
				curve_value c2{ c };
				ensure(c2 == c);
				c = c2;
				ensure(!(c != c2));

				ensure(std::isnan(c(-1.)));
				for (size_t i = 0; i < 3; ++i) {
					ensure(f[i] == c(t[i]));
				}
				ensure(f[0] == c(0.5));
				ensure(f[1] == c(1.5));
				ensure(f[2] == c(2.5));
				ensure(std::isnan(c(3.5)));
			}
			/*
			{
				curve_value c(3, t, f);
				c.translate(0.5);
				ensure(f[0] == c(0.5));
				ensure(f[1] == c(1.5));
			}
			*/
			return 0;
		}

#endif // _DEBUG

	};

}