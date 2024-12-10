// tmx_variate_normal.h - Standard normal random variate.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <cmath>
#include <numbers>
#include "tmx_variate.h"

namespace tmx::variate {

	template<class X = double, class S = double>
	struct normal : public interface<X,S> {

		static X pdf(X x)
		{
			static X sqrt2pi = std::sqrt(2 * std::numbers::pi);

			return std::exp(-x * x / 2) / sqrt2pi;

		}
		// standard normal share density
		X _pdf(X x, S s) const override
		{
			return pdf(x - s);
		}

		static X cdf(X x)
		{
			return 0.5 * (1 + std::erf(x / std::numbers::sqrt2_v<X>));
		}
		// standard normal share cumulative distribution function
		X _cdf(X x, S s) const override
		{
			return cdf(x - s);
		}


		S _cgf(S s) const override
		{
			return s * s / 2;
		}

		S _mgf(S s) const override
		{
			return std::exp(s * s / 2);
		}
#ifdef _DEBUG
		static int test()
		{
			{
				normal<> n;
				assert(n.pdf(0) == 1 / std::sqrt(2 * std::numbers::pi));
				assert(n.cdf(0) == 0.5);
				assert(n.cgf(0) == 0);
				assert(n.mgf(0) == 1);

				interface<>& b = n;
				assert(b.pdf(0) == 1 / std::sqrt(2 * std::numbers::pi));
				assert(b.cdf(0) == 0.5);
				assert(b.cgf(0) == 0);
				assert(b.mgf(0) == 1);
			}

			return 0;
		}
#endif // _DEBUG
		// P(B_t <= x, min_{0<=s<=t} B_s <= y), y <= 0
		// E[f(B_t) 1(min_{0<=s<=t} B_s <= y)] = E[(f(B_t) + f(2y - B_t)) 1(B_t <= y)]
		static X joint_min(X x, X y)
		{
			if (y >= 0) { // min_{0<=s<=t} B_s <= 0
				return cdf(x);
			}

			X p = cdf(min(x, y));
			if (y <= x) { // 2y - x <= y
				p += cdf(y) - cdf(2y - x);
			}

			return p;
		}
		// P(B_t <= x, max_{0<=s<=t} B_s <= y), y >= 0
		// E[f(B_t) 1(max_{0<=s<=t} B_s > y)] = E[(f(B_t) + f(2y - B_t)) 1(B_t > y)]
		static X joint_max(X x, X y)
		{
			if (y >= 0) { // min_{0<=s<=t} B_s <= 0
				return cdf(x);
			}

			X p = cdf(min(x, y));
			if (y <= x) { // 2y - x <= y
				p += cdf(y) - cdf(2y - x);
			}

			return p;
		}
	};

} // namespace tmx::variate
