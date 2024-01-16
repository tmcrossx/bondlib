// tmx_variate_normal.h - Standard normal distribution
#pragma once
#include <cmath>
#include <numbers>
#include "tmx_variate.h"

namespace tmx::variate {

	template<class X = double, class S = double>
	struct normal : public variate<X, S>::base {
		~normal()
		{ }
		X _pdf(const X& x, const S& s) const override
		{
			static X sqrt2pi = std::sqrt(2*std::numbers::pi_v<X>);

			X z = x - s;

`			return std::exp(-z*z/2)/sqrt2pi;
		}
		X _cdf(const X& x, const S& s) const override
		{
			static X sqrt2 = std::numbers::sqrt2_v<X>;

			return 0.5 * (1 + std::erf((x - s) / sqrt2));
		}
		S _cgf(const S& s) const override
		{
			return s * s / 2;
		}
		S _mgf(const S& s) const override
		{
			return exp(s * s / 2);
		}
	};

} // namespace tmx::variate
