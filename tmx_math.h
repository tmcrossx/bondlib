// tmx_math.h - constexpr math functions
#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <functional>
#include <limits>
#include "tmx_math_limits.h"

namespace tmx::math {

	// Sign of x.
	template<class X>
	constexpr X sgn(X x)
	{
		return x > 0 ? X(1) : x < 0 ? X(-1) : X(0);
	}
#ifdef _DEBUG
	static_assert(sgn(-2) == -1);
	static_assert(sgn(0) == 0);
	static_assert(sgn(2) == 1);
#endif // _DEBUG

	template<class X>
	constexpr bool samesign(X x, X y)
	{
		//return x = std::copysign(x, y);
		return sgn(x) == sgn(y);
	}
#ifdef _DEBUG
	static_assert(samesign(-2, -3));
	static_assert(samesign(0, 0));
	static_assert(samesign(2, 3));
	static_assert(!samesign(-2, 3));
	static_assert(!samesign(2, -3));
#endif // _DEBUG

	template<class X>
	constexpr X fabs(X x)
	{
		return x >= 0 ? x : -x;
	}
#ifdef _DEBUG
	static_assert(fabs(-1) == 1);
	static_assert(fabs(1) == 1);
#endif // _DEBUG

	template<class X>
	constexpr X pow(X x, int n)
	{
		return n == 0 ? 1 : n > 0 ? x * pow(x, n - 1) : 1 / pow(x, -n);
	}
#ifdef _DEBUG
	static_assert(pow(1, 1) == 1);
	static_assert(pow(2, 3) == 8);
	static_assert(pow(2., -3) == 1./8);
	static_assert(pow(2, -3) == 0);
	static_assert(pow(.5, 2) == .25);
	static_assert(pow(.5, -2) == 1/.25);
	static_assert(pow(.1, 2) == .1 * .1);
	static_assert(pow(.1, -2) == 1 / (.1 * .1));
	static_assert(pow(.1, -2) == 1 / pow(.1, 2));
#endif // _DEBUG


	// Equal up to n digits of precision.
	template<class X>
	constexpr bool equal_precision(X x, X y, int n)
	{
		return fabs(x - y) <= pow(10., n);
	}

	template<class X>
	constexpr X sqrt(X a, X x)
	{
		if (a < 0) {
			return NaN<X>;
		}

		if (a == 0) {
			return 0;
		}

		while (fabs(x * x - a) > a * epsilon<X>) {
			x = x / 2 + a / (2 * x);
		}

		return x;
	}
	template<class X>
	constexpr X sqrt(X a)
	{
		return sqrt(a, a / 2);
	}
#ifdef _DEBUG
	static_assert(sqrt(4.) == 2);
	static_assert(sqrt(1e-16) == 1e-8);
#endif // _DEBUG

	template<class X>
	constexpr X sqrt_epsilon = sqrt(std::numeric_limits<X>::epsilon());
	template<class X>
	constexpr X phi = (X(1) + sqrt(X(5))) / X(2);

	// (1 + x)^a = 1 + ax + a(a - 1)/2 x^2 + ... = sum_{n>=0}(a)_n x^n/n!
	template<class X>
	constexpr X pow1(X x, X a, X eps = sqrt_epsilon<X>)
	{
		if (fabs(x) == 1) {
			return infinity<X>;
		}

		X s = 1;
		X n = 1;
		X anx = 1; // (a)_n x^n/n!
		while (fabs(anx) > eps) {
			anx *= (a - n + 1) * x / n;
			s += anx;
			n = n + 1;
		}

		return s;		
	}
#ifdef _DEBUG
	static_assert(pow1(0., 1.) == 1);
	static_assert(pow1(-0.5, 2.) == 0.25);
	constexpr double xxx = pow1(-0.5, -2.);
	//static_assert(pow1(-0.5, -2.) == 4);
#endif // _DEBUG

	// Estimate f'(x) using forward difference.
	template<class F, class X>
	constexpr X forward_difference(const F& f, X x, X h = sqrt_epsilon<X>)
	{
		return (f(x + h) - f(x)) / h;
	}
	// Estimate f'(x) using backward difference.
	template<class F, class X>
	constexpr X backward_difference(const F& f, X x, X h = sqrt_epsilon<X>)
	{
		return (f(x) - f(x - h)) / h;
	}
	// Estimate f'(x) using symmetric difference.
	template<class F, class X>
	constexpr X symmetric_difference(const F& f, X x, X h = sqrt_epsilon<X>)
	{
		return (f(x + h) - f(x - h)) / (2 * h);
	}
#ifdef _DEBUG
	static_assert(symmetric_difference([](double x) { return x * x; }, 1.) == 2);
	static_assert(symmetric_difference([](double x) { return x * x; }, 2.) == 4);
#endif // _DEBUG

	// Estimate f''(x) using symmetric differences.
	template<class F, class X>
	constexpr X second_difference(const F& f, X x, X h = sqrt_epsilon<X>)
	{
		return (f(x + h) - 2*f(x) + f(x - h)) / (h * h);
	}

} // namespace tmx::math