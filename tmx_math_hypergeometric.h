// tmx_math_hypergeometric.h : Generalized hypergeometric function and its derivatives.
#pragma once
#include "tmx_math.h"

namespace tmx::math {

	// Generalized hypergeometric function and its derivatives.
	// pFq(p0, ...; q0, ...; x) = sum_{n>=0} (p0)_n ... /(q0)_n ... x^n/n!
	// (q)_n = q(q + 1)...(q + n - 1), (q)_0 = 1
	// (d/dx)^n pFq(p, q, x) = (p)_n/(q_n) pFq(p + n, q + n, x)
	template<class X = double>
	constexpr X pFq(unsigned np, const X* p, unsigned nq, const X* q, X x, X eps = sqrt_epsilon<X>/*, int dn = 0 */)
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
		for (unsigned n = 1; fabs(ds) > eps || fabs(x_n) > eps; ++n) {
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
#ifdef _DEBUG
	static_assert(pFq(0, (const double*)nullptr, 0, (const double*)nullptr, 0.) == 1);
#endif // _DEBUG

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
#ifdef _DEBUG
	// constexpr double ee = math::exp(1.);
	static_assert(exp(1.) == 2.7182818282861687); 
	                            // 2.71828182845904523536028747135266249775724709369995
#endif // _DEBUG

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
//#if 0
	template<class X = double>
	inline int hypergeometric_test()
	{
		{
			// (1 - x)^{-a} = 1 + ax + a(a + 1)/2 x^2 + ... = sum_{n>=0}(a)_n x^n/n!
			constexpr X as[] = { -0.1, 0, 0.1, 1, 2 };
			constexpr X xs[] = { -0.5, 0, 0.5 };
			for (X a : as) {
				for (X x : xs) {
					constexpr X F = pow1(-x, -a); // 1F0(a,x)
					constexpr X _F = pFq(1, &a, 0, (const X*)nullptr, x); // 1F0(x)
					constexpr X dF = F - _F;
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
				static_assert(fabs(_F - F) < epsilon<X>);
			}
		}

		return 0;
	}
//#endif // 0
#endif // _DEBUG
} // namespace tmx::math
