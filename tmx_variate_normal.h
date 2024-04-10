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
	struct normal : public base<X,S> {

		// standard normal share density
		X _pdf(X x, S s) const override
		{
			static X sqrt2pi = std::sqrt(2 * std::numbers::pi);
			
			X x_ = x - s;

			return std::exp(-x_ * x_ / 2) / sqrt2pi;
		}

		// standard normal share cumulative distribution function
		X _cdf(X x, S s) const override
		{
			static X sqrt2 = std::numbers::sqrt2_v<X>;

			return 0.5 * (1 + std::erf((x - s) / sqrt2));
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

				base<>& b = n;
				assert(b.pdf(0) == 1 / std::sqrt(2 * std::numbers::pi));
				assert(b.cdf(0) == 0.5);
				assert(b.cgf(0) == 0);
				assert(b.mgf(0) == 1);
			}

			return 0;
		}
#endif // _DEBUG
	};

} // namespace tmx::variate
