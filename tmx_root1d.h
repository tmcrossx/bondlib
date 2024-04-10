// fmx_root1d.h - 1-d root using secant method
#pragma once
#include <cmath>
#include <limits>
#include "tmx_math.h"

namespace tmx::root1d {

	template<class X>
	struct secant {
		X x0, x1;
		X tolerance;
		size_t iterations;

		secant(X x0, X x1, X tol = math::sqrt_epsilon<X>, size_t iter = 100)
			: x0(x0), x1(x1), tolerance(tol), iterations(iter)
		{ }

		template<class X, class Y>
		constexpr auto next(X x0, Y y0, X x1, Y y1)
		{
			return (x0 * y1 - x1 * y0) / (y1 - y0);
		}

		// Find root given two initial guesses.
		template<class F, class X = double, class Y = double>
		constexpr X solve(const F& f)
		{
			Y y0 = f(x0);
			Y y1 = f(x1);
			bool bounded = !math::samesign(y0, y1);

			while (iterations-- and math::fabs(y1) > tolerance) {
				auto x = next(x0, y0, x1, y1);
				Y y = f(x);
				if (bounded and math::samesign(y, y1)) {
					x1 = x;
					y1 = y;
				}
				else {
					x0 = x1;
					y0 = y1;
					x1 = x;
					y1 = y;
					bounded = !math::samesign(y0, y1);
				}
			}

			return iterations ? x1 : math::NaN<X>;
		}
#ifdef _DEBUG
		constexpr int solve_test()
		{
			{
				constexpr double x = solve([](double x) { return x * x - 4; }, 0., 1.);
				static_assert(math::fabs(x - 2) <= math::sqrt_epsilon<double>);
			}

			return 0;
		}
#endif // _DEBUG
	};

	template<class X = double, class Y = double>
	struct newton {
		X x0;
		X tolerance;
		size_t iterations = 100;

		newton(X x0, X tol = math::sqrt_epsilon<X>, size_t iter = 100)
			: x0(x0), tolerance(tol), iterations(iter)
		{ }

		template<class X, class Y>
		constexpr auto next(X x0, Y y0, Y dy)
		{
			return x0 - y0 / dy;
		}

		// Find positive root given initial guess and derivative.
		template<class F, class dF>
		constexpr X solve(const F& f, const dF& df)
		{
			auto y0 = f(x0);
			while (iterations-- and math::fabs(y0) > tolerance) {
				auto x = next(x0, y0, df(x0));
				x0 = x > 0 ? x : x0 / 2;
				y0 = f(x0);
			}

			return iterations ? x0 : math::NaN<X>;
		}
#ifdef _DEBUG
		constexpr int solve_test()
		{
			{
				constexpr double x = solve([](double x) { return x * x - 4; }, [](double x) { return 2 * x; }, 1.);
				static_assert(math::fabs(x - 2) < math::sqrt_epsilon<double>);
			}

			return 0;
		}
#endif // _DEBUG	
	};

} // namespace fms::secant


#if 0
	// Return x0 <= a, b <= x1 with f(a) and f(b) different sign.
template<class F, class X>
constexpr std::pair<X, X> bound(const F& f, X a, X b, X x0 = -infinity<X>, X x1 = infinity<X>)
{
	X fa = f(a);
	X fb = f(b);

	while (samesign(fa, fb)) {
		X d = b - a;
		if (fabs(fa) > fabs(fb)) {
			a = b;
			b = b + 2 * d;
			if (x1 != infinity<X> and b > x1) {
				b = (a + x1) / 2;
			}
		}
		else {
			b = a;
			a = a - 2 * d;
			if (x0 != -infinity<X> and a < x0) {
				a = (x0 + b) / 2;
			}
		}
	}

	if (x0 != -infinity<X> and x0 == a)
		return { NaN<X>, b };
	if (x1 != infinity<X> and x1 == b)
		return { a, NaN<X> };

	return { a, b };
}

#ifdef _DEBUG
template<class X>
inline int bound_test()
{
	constexpr auto f = [](X x) { return x; };
	{
		static_assert(std::make_pair<X, X>(-1, 1) == bound(f, X(-1), X(1)));
	}

	return 0;
}
#endif // _DEBUG
#endif // 0

