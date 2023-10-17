// fmx_root1d.h - 1-d root using secant method
#pragma once
#include <cmath>
#include <limits>

namespace tmx::root1d {

	template<class X>
	constexpr X NaN = std::numeric_limits<X>::quiet_NaN();
	template<class X>
	constexpr X epsilon = std::numeric_limits<X>::epsilon();
	template<class X>
	constexpr X infinity = std::numeric_limits<X>::infinity();

	template<class X>
	constexpr X sqrt(X a)
	{
		if (a < 0) {
			return std::numeric_limits<X>::quiet_NaN();
		}

		if (a == 0) {
			return 0;
		}

		X x = a; // bad initial guess
		X x_ = x / 2 + a / (2 * x);

		while (x_ - x > 2*epsilon<X> or x_ - x < -2*epsilon<X>) {
			x = x_;
			x_ = x / 2 + a / (2 * x);
		}

		return x_;
	}
	template<class X>
	constexpr X sqrt_epsilon = sqrt(std::numeric_limits<X>::epsilon());
	template<class X>
	constexpr X phi = (X(1) + sqrt(X(5))) / X(2);

	template<class X>
	constexpr bool samesign(X x, X y)
	{
		//return x = std::copysign(x, y);
		return (x > 0 and y > 0) or (x < 0 and y < 0);
	}

	template<class X>
	constexpr X fabs(X x)
	{
		return x >= 0 ? x : -x;
	}

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

	namespace secant {

		template<class X, class Y>
		constexpr auto next(X x0, Y y0, X x1, Y y1)
		{
			return (x0 * y1 - x1 * y0) / (y1 - y0);
		}

		// Find root given two initial guesses.
		template<class F, class X = double>
		constexpr X solve(const F& f, X x0, X x1,
			X tol = sqrt_epsilon<X>, int iter = 100)
		{
			auto y0 = f(x0);
			auto y1 = f(x1);
			bool bounded = !samesign(y0, y1);

			while (iter-- and (y0 > tol or y0 < -tol)) {
				auto x = next(x0, y0, x1, y1);
				auto y = f(x);
				if (bounded and samesign(y, y1)) {
					x1 = x;
					y1 = y;
					//assert(!samesign(y0, y1));
				}
				else {
					x0 = x1;
					y0 = y1;
					x1 = x;
					y1 = y;
					bounded = !samesign(y0, y1);
				}
			}

			return iter ? x0 : NaN<X>;
		}
#ifdef _DEBUG
		constexpr int secant_solve_test()
		{
			{
				constexpr double x = solve([](double x) { return x * x - 4; }, 0., 1.);
				static_assert(x >= 2 - sqrt(epsilon<double>));
				static_assert(x <= 2 + sqrt(epsilon<double>));
			}

			return 0;
		}
#endif // _DEBUG
	}

	namespace newton {

		template<class X, class Y>
		constexpr auto next(X x0, Y y0, Y dy)
		{
			return x0 - y0 / dy;
		}

		// Find positive root given initial guess and derivative.
		template<class F, class dF, class X = double>
		constexpr X solve(const F& f, const dF& df, X x0,
			double tol = sqrt_epsilon<X>, int iter = 100)
		{
			auto y0 = f(x0);
			while (iter-- and (y0 > tol or y0 < -tol)) {
				auto x = next(x0, y0, df(x0));
				x0 = x > 0 ? x : x0 / 2;
				y0 = f(x0);
			}

			return iter ? x0 : NaN<X>;
		}
#ifdef _DEBUG
		constexpr int newton_solve_test()
		{
			{
				constexpr double x = solve([](double x) { return x * x - 4; }, [](double x) { return 2 * x; }, 1.);
				static_assert(x >= 2 - sqrt(epsilon<double>));
				static_assert(x <= 2 + sqrt(epsilon<double>));
			}

			return 0;
		}
#endif // _DEBUG	
	}

} // namespace fms::secant