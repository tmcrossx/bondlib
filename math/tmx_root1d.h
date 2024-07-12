// fmx_root1d.h - 1-d root using secant method
#pragma once
#include <tuple>
#include "tmx_math.h"

namespace tmx::root1d {

	// Move x to bracketed root in [a, b] given last bracketed guess x0.
	template<class X>
	constexpr X bracket(X x, X x0, X a, X b)
	{
		if (a >= x0) {
			throw __FUNCTION__ "x0 is less than a";
		}
		if (x0 >= b) {
			throw __FUNCTION__ "x0 is greater than b";
		}

		if (x < a) {
			return (x0 + a)/2;
		}
		else if (x > b) {
			return (x0 + b)/2;
		}

		return x;
	}

	template<class X, class Y>
	struct secant {
		X x0, x1;
		X tolerance;
		size_t iterations;

		secant(X x0, X x1, X tol = math::sqrt_epsilon<X>, size_t iter = 100)
			: x0(x0), x1(x1), tolerance(tol), iterations(iter)
		{ }

		constexpr auto next(X x0, Y y0, X x1, Y y1)
		{
			return (x0 * y1 - x1 * y0) / (y1 - y0);
		}

		// Find root given two initial guesses.
		// Return root approximation, tolerance, and number of iterations.
		template<class F>
		constexpr std::tuple<X,X,size_t> solve(const F& f)
		{
			Y y0 = f(x0);
			Y y1 = f(x1);
			bool bounded = !math::samesign(y0, y1);
			size_t n = 0;

			while (++n < iterations && math::fabs(y1) > tolerance) {
				auto x = next(x0, y0, x1, y1);
				Y y = f(x);
				if (bounded && math::samesign(y, y1)) {
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
			if (n == iterations) {
				x1 = std::numeric_limits<X>::quiet_NaN();
			}

			return { x1, y1, n };
		}
#ifdef _DEBUG
		constexpr int solve_test()
		{
			{
				constexpr double x = std::get<0>(solve([](double x) { return x * x - 4; }, 0., 1.));
				static_assert(math::fabs(x - 2) <= math::sqrt_epsilon<double>);
			}

			return 0;
		}
#endif // _DEBUG
	};

	// Find root given initial guess and derivative using Newton's method.
	// Return root approximation, tolerance, and number of iterations.
	template<class X = double, class Y = double>
	struct newton {
		X x0;
		X tolerance;
		size_t iterations = 100;

		newton(X x0, X tol = math::sqrt_epsilon<X>, size_t iter = 100)
			: x0(x0), tolerance(tol), iterations(iter)
		{ }

		constexpr auto next(X x, Y y, decltype(y/x) df)
		{
			return x - y / df;
		}

		// Find positive root in [a, b] given initial guess and derivative.
		template<class F, class dF>
		constexpr std::tuple<X,X,size_t> solve(const F& f, const dF& df, X a = -math::infinity<X>, X b = math::infinity<X>)
		{
			auto y0 = f(x0);
			size_t n = 0;
			while (++n < iterations && math::fabs(y0) > tolerance) {
				auto x = next(x0, y0, df(x0));
				x0 = bracket(x, x0, a, b);
				y0 = f(x0);
			}
			if (n == iterations) {
				x0 = std::numeric_limits<X>::quiet_NaN();
			}

			return { x0, y0, n };
		}
#ifdef _DEBUG
		constexpr int solve_test()
		{
			{
				constexpr double x = std::get<0>(solve([](double x) { return x * x - 4; }, [](double x) { return 2 * x; }, 1.));
				static_assert(math::fabs(x - 2) < math::sqrt_epsilon<double>);
			}

			return 0;
		}
#endif // _DEBUG	
	};

} // namespace fms::secant


