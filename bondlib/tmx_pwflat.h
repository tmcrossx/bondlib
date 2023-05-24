// tmx_pwflat.h - piecewise flat curve
/*
		   { f[i] if t[i-1] < t <= t[i];
	f(t) = { _f   if t > t[n-1];
		   { NaN  if t < 0
	F                                   _f
	|        f[1]             f[n-1] o--------
	| f[0] o------          o--------x
	x------x      ... ------x
	|
	0-----t[0]--- ... ---t[n-2]---t[n-1]--- T
*/
#pragma once
#include <cmath>
#include <algorithm>
#include <iterator>
#include <limits>
#include <numeric>
#include <span>
#include <vector>
#include "ensure.h"

namespace tmx::pwflat {

	template<class X>
	inline static constexpr X NaN = std::numeric_limits<X>::quiet_NaN();

	// strictly increasing values
	template<class I>
	constexpr bool monotonic(const I b, const I e)
	{
		using T = std::iterator_traits<I>::value_type;

		return e == std::adjacent_find(b, e, std::greater_equal<T>{});
	}
	template<class T>
	constexpr bool monotonic(size_t n, const T* t)
	{
		return monotonic(t, t + n);
	}
	template<class T>
	constexpr bool monotonic(const std::span<T>& t)
	{
		return monotonic(t.begin(), t.end());
	}
	template<class T>
	constexpr bool monotonic(const std::initializer_list<T>& t)
	{
		return monotonic(t.begin(), t.end());
	}

#ifdef _DEBUG

	inline int test_pwflat_monotonic()
	{
		static_assert(monotonic<int>(0,nullptr));
		static_assert(monotonic({ 1 }));
		static_assert(monotonic({ 1,2 }));
		static_assert(!monotonic({ 1,1 }));

		return 0;
	}

#endif // _DEBUG

	// f(u) assuming t[i] monotonically increasing
	template<class T = double, class F = double>
	constexpr F value(T u, size_t n, const T* t, const F* f, F _f = NaN<F>)
	{
		if (u < 0)
			return NaN<F>;
		if (n == 0)
			return _f;

		auto ti = std::lower_bound(t, t + n, u);

		return ti == t + n ? _f : f[ti - t];
	}

#ifdef _DEBUG

	inline int test_pwflat_value()
	{
		constexpr double t[] = { 1, 2, 3 };
		constexpr double f[] = { 2, 3, 4 };
		{
			{
				constexpr double v = value(0., 0, t, f);
				static_assert(v != v); // std::isnan not constexpr in VS2022
			}
			static_assert(5 == value(0., 0, t, f, 5.));
			{
				constexpr double v = value(-0.1, 3, t, f);
				static_assert(v != v);
			}
			static_assert(2 == value(0., 3, t, f));
			static_assert(2 == value(0.1, 3, t, f));
			static_assert(2 == value(1., 3, t, f));
			static_assert(3 == value(1.1, 3, t, f));
			static_assert(4 == value(2.9, 3, t, f));
			static_assert(4 == value(3., 3, t, f));
			{
				constexpr double v = value(3.1, 3, t, f);
				static_assert(v != v);
			}
		}

		return 0;
	}

#endif // _DEBUG

	// int_0^u f(t) dt
	template<class T, class F>
	constexpr F integral(T u, size_t n, const T* t, const F* f, F _f = NaN<F>)
	{
		if (u < 0)
			return NaN<F>;
		if (u == 0)
			return 0;
		if (n == 0)
			return u * _f;

		F I = 0;
		T t_ = 0;

		size_t i;
		for (i = 0; i < n and t[i] <= u; ++i) {
			I += f[i] * (t[i] - t_);
			t_ = t[i];
		}
		if (u > t_) {
			I += (i == n ? _f : f[i]) * (u - t_);
		}

		return I;
	}

#ifdef _DEBUG

	inline int test_pwflat_integral()
	{
		constexpr double t[] = { 1, 2, 3 };
		constexpr double f[] = { 2, 3, 4 };
		{
		
			static_assert(0 == integral(0., 0, t, f));
			static_assert(10 == integral(2., 0, t, f, 5.));
			{
				constexpr double v = integral(-0.1, 3, t, f);
				static_assert(v != v);
			}
			static_assert(0 == integral(0., 3, t, f));
			static_assert(1 == integral(0.5, 3, t, f));
			static_assert(2 == integral(1., 3, t, f));
			static_assert(2 + 3./2 == integral(1.5, 3, t, f));
			static_assert(5 == integral(2., 3, t, f));
			static_assert(9 == integral(3., 3, t, f));
			//ensure(9 == integral(3., 3, t, f));
			{
				constexpr double v = integral(3.1, 3, t, f);
				static_assert(v != v);
			}
		}

		return 0;
	}

#endif // _DEBUG

	// discount D(u) = exp(-int_0^u f(t) dt)
	template<class T, class F>
	constexpr F discount(T u, size_t n, const T* t, const F* f, F _f = NaN<F>)
	{
		return exp(-integral(u, n, t, f, _f));
	}

#ifdef _DEBUG

	inline int test_pwflat_discount()
	{
		constexpr double t[] = { 1, 2, 3 };
		constexpr double f[] = { 2, 3, 4 };
		{
			/*
			static_assert(0 == discount(0., 0, t, f));
			static_assert(10 == discount(2., 0, t, f, 5.));
			{
				constexpr double v = discount(-0.1, 3, t, f);
				static_assert(v != v);
			}
			static_assert(0 == discount(0., 3, t, f));
			static_assert(1 == discount(0.5, 3, t, f));
			static_assert(2 == discount(1., 3, t, f));
			static_assert(2 + 3. / 2 == discount(1.5, 3, t, f));
			static_assert(5 == discount(2., 3, t, f));
			//!!!failing static_assert(9 == discount(3., 3, t, f));
			ensure(9 == discount(3., 3, t, f));
			{
				constexpr double v = discount(3.1, 3, t, f);
				static_assert(v != v);
			}
			*/
		}

		return 0;
	}

#endif // _DEBUG

	// spot r(u) = (int_0^u f(t) dt)/u
	// r(u) = f(u) if u <= t[0]
	template<class T, class F>
	constexpr F spot(T u, size_t n, const T* t, const F* f, F _f = NaN<F>)
	{
		return n == 0 ? _f
			: u <= t[0] ? value(u, n, t, f, _f) : integral(u, n, t, f, _f) / u;
	}

#ifdef _DEBUG

	inline int test_pwflat_spot()
	{
		constexpr double t[] = { 1, 2, 3 };
		constexpr double f[] = { 2, 3, 4 };
		{
			/*
			static_assert(0 == spot(0., 0, t, f));
			static_assert(10 == spot(2., 0, t, f, 5.));
			{
				constexpr double v = spot(-0.1, 3, t, f);
				static_assert(v != v);
			}
			static_assert(0 == spot(0., 3, t, f));
			static_assert(1 == spot(0.5, 3, t, f));
			static_assert(2 == spot(1., 3, t, f));
			static_assert(2 + 3. / 2 == spot(1.5, 3, t, f));
			static_assert(5 == spot(2., 3, t, f));
			//!!!failing static_assert(9 == spot(3., 3, t, f));
			ensure(9 == spot(3., 3, t, f));
			{
				constexpr double v = spot(3.1, 3, t, f);
				static_assert(v != v);
			}
			*/
		}

		return 0;
	}

#endif // _DEBUG

}