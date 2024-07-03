// fms_pwflat.h - piecewise flat right-continuous forward curve determined by points (t[i], f[i]).
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

	Note f(t[i]) = f[i].
*/
#pragma once
#include <cmath>
#include <algorithm>
#include <limits>
#include <numeric>
#include <iterator>
#include <vector>
#include "math/tmx_math.h"

namespace tmx {
	namespace pwflat {

		// strictly increasing values
		template<class T>
		constexpr bool monotonic(size_t n, const T* t)
		{
			return t + n == std::adjacent_find(t, t + n, std::greater_equal<T>{});
		}
#ifdef _DEBUG
		inline int monotonic_test()
		{
			{
				static_assert(monotonic(0, (int*)0));
			}
			{
				static constexpr double t[] = { 1,2,3 };
				static_assert(monotonic(3, t));
			}
			{
				static constexpr double t[] = { 1,1,3 };
				static_assert(!monotonic(3, t));
			}

			return 0;
		}	
#endif // _DEBUG

		// f(u) assuming t is monotonically increasing
		template<class T = double, class F = double>
		constexpr F forward(T u, size_t n, const T* t, const F* f, F _f = math::NaN<F>)
		{
			if (u < 0)  return math::NaN<F>;
			if (n == 0) return _f;

			auto ti = std::lower_bound(t, t + n, u); // greatest i with t[i] <= u

			return ti == t + n ? _f : f[ti - t];
		}
#ifdef _DEBUG
#define IS_NAN(x) x != x
		inline int forward_test()
		{
			{
				static constexpr double t[] = { 1,2,3 };
				static constexpr double f[] = { 4,5,6 };
				static_assert(IS_NAN(forward(-1., 3, t, f)));
				static_assert(f[0] == forward(0., 3, t, f));
				static_assert(f[0] == forward(t[0], 3, t, f));
				static_assert(f[1] == forward(1.5, 3, t, f));
				static_assert(f[1] == forward(t[1], 3, t, f));
				static_assert(f[2] == forward(t[2], 3, t, f));
				static_assert(IS_NAN(forward(t[2] + 1, 3, t, f)));
			}

			return 0;
		}
#undef IS_NAN
#endif // _DEBUG

		// TODO: int_0^u f(t) dt
		// Integral from 0 to u of f.
		template<class T, class F>
		constexpr F integral(T u, size_t n, const T* t, const F* f, F _f = math::NaN<F>)
		{
			if (u < 0)  return math::NaN<F>;
			if (u == 0) return 0;
			if (n == 0) return u * _f;

			F I = 0;
			T t_ = 0;

			size_t i;
			for (i = 0; i < n && t[i] <= u; ++i) {
				I += f[i] * (t[i] - t_);
				t_ = t[i];
			}
			if (u > t_) {
				I += (i == n ? _f : f[i]) * (u - t_);
			}

			return I;
		}
#ifdef _DEBUG
#define IS_NAN(x) (x) != (x)
		inline int integral_test()
		{
			{
				static constexpr double t[] = { 1,2,3 };
				static constexpr double f[] = { 4,5,6 };
				static_assert(IS_NAN(integral(-1., 3, t, f)));
				static_assert(integral(0., 3, t, f) == 0);
				static_assert(integral(0.5, 3, t, f) == 4*0.5);
				static_assert(integral(1., 3, t, f) == 4);
				static_assert(integral(1.5, 3, t, f) == 4 + 5*0.5);
				static_assert(integral(2., 3, t, f) == 4 + 5);
				static_assert(integral(2.5, 3, t, f) == 4 + 5 + 6*0.5);
				static_assert(integral(3., 3, t, f) == 4 + 5 + 6);
				static_assert(IS_NAN(integral(3.1, 3, t, f)));
				static_assert(integral(3.5, 3, t, f, 7.) == 4 + 5 + 6 + 7*0.5);
			}

			return 0;
		}
#undef IS_NAN
#endif // _DEBUG

		// discount D(u) = exponential(-int_0^u f(t) dt)
		template<class T, class F>
		constexpr F discount(T u, size_t n, const T* t, const F* f, F _f = math::NaN<F>)
		{
			return exp(-integral(u, n, t, f, _f));
		}

		// spot r(u) = (int_0^u f(t) dt)/u
		// r(u) = f(u) if u <= t[0]
		template<class T, class F>
		constexpr F spot(T u, size_t n, const T* t, const F* f, F _f = math::NaN<F>)
		{
			if (n == 0) return _f;
			
			return u <= t[0] ? forward(u, n, t, f, _f) : integral(u, n, t, f, _f) / u;
		}

	} // namespace pwflat
} // namespace fms
