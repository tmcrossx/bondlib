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

	// Generalized hypergeometric function and its derivatives.
	// pFq(p0, ...; q0, ...; x) = sum_{n>=0} (p0)_n ... /(q0)_n ... x^n/n!
	// (q)_n = q(q + 1)...(q + n - 1), (q)_0 = 1
	// (d/dx)^n pFq(p, q, x) = (p)_n/(q_n) pFq(p + n, q + n, x)
	template<class X = double>
	constexpr X pFq(unsigned np, const X* p, unsigned nq, const X* q, X x, X eps = sqrt_epsilon<X>/*, int dn = 0 */ )
	{
		if (np > nq + 1) { // diverges
			return x == 0 ? 0 : std::numeric_limits<X>::infinity();
		}
		if (np == nq + 1 and abs(x) >= 1) {
			return std::numeric_limits<X>::infinity();
		}

		X p_qn = 1; // (p0)_n .../(q0)_n ...
		X x_n = 1;  // x^n/n!
		X s = 1;    // n = 0

		X ds = 1;
		for (unsigned n = 1; abs(ds) > eps || abs(x_n) > eps; ++n) {
			unsigned int i = 0;
			while (i < np && i < nq) {
				p_qn *= (p[i] + n - 1) / (q[i] + n - 1);
				++i;
			}
			while (i < np) {
				p_qn *= p[i] + n - 1;
				++i;
			}
			while (i < nq) {
				p_qn /= q[i] + n - 1;
				++i;
			}
			x_n *= x / n;
			ds = p_qn * x_n;
			s += ds;
		}

		return s;
	}

	//
	// Well known functions
	//
	template<class X = double>
	constexpr X _1F1(X p, X q, X x, X eps)
	{
		return pFq(1, &p, 1, &q, x, eps);
	}
	template<class X>
	constexpr X exp(X x, X eps = sqrt_epsilon<X>)
	{
		return _1F1(1., 1., x, eps);
	}

	template<class X = double>
	constexpr X _2F1(X a, X b, X c, X x, X eps)
	{
		X _p[2] = { a, b };

		return pFq(2, _p, 1, &c, x, eps);
	}

	// (lower) incomplete gamma function
	// int_0^x t^{a - 1} e^{-t} dt 
	template<class X>
	constexpr X incomplete_gamma(X a, X x, X eps)
	{
		return (pow(x, a) * exp(x, eps) / a) * _1F1(1, 1 + a, x, eps);
	}

#ifdef _DEBUG
#if 0
	template<class X = double>
	inline int hypergeometric_test()
	{
		{
			// (1 - x)^{-a} = 1 + ax + a(a + 1)/2 x^2 + ... = sum_{n>=0}(a)_n x^n/n!
			X as[] = { -0.1, 0, 0.1, 1, 2 };
			X xs[] = { -0.5, 0, 0.5 };
			for (X a : as) {
				for (X x : xs) {
					X F = pow1(-x, -a); // 1F0(a,x)
					X _F = pFq(1, &a, 0, (const X*)nullptr, x); // 1F0(x)
					X dF = F - _F;
					static_assert(fabs(dF) < 2 * epsilon<X>);
				}
			}
		}
		{
			// _2F1(1, 1, 2, -x) = log(1 + x)/x;
			X one = 1;
			X two = 2;
			double xs[] = { -.1, .1, .9 };

			for (X x : xs) {
				X F = _2F1(one, one, two, -x, epsilon<X>);
				X F_ = log(1 + x) / x;
				X dF = F - F_;
				static_assert(fabs(dF) < epsilon<X>);
				X one_[] = { 1, 1 };
				X two_[] = { 2 };
				X _F = pFq(2, one_, 1, two_, -x);
				X Fd = _F - F;
				static_assert(fabs(_F - F) < std::numeric_limits<X>::epsilon());
			}
		}

		return 0;
	}
#endif // 0
#endif // _DEBUG
} // namespace tmx::math