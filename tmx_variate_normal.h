// tmx_variate_normal.h - Standard normal random variate.
#pragma once
#include <cmath>
#include "tmx_variate.h"

namespace tmx::variate {

	template<class X = double, class S = double>
	struct normal : public base<X,S> {

		// standard normal share density
		X _pdf(X x, S s = 0) const override
		{
			static X sqrt2pi = std::sqrt(2 * std::numbers::pi_v<X>);
			X z = x - s;

			return std::exp(-z * z / 2) / sqrt2pi;
		}

		// standard normal share cumulative distribution function
		X _cdf(X x, S s = 0) const override
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

	} // namespace normal


} // namespace tmx::variate
