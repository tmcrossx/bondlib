// tmx_black.h - Fischer Black model
// F = f exp(sZ - s^2/2) where Z is standard normal
#pragma once
#include <cmath>
#include <numbers>
//#include "ensure.h"
#include "tmx_root1d.h"

namespace tmx::black {

	namespace normal {
		// standard normal share density
		template<class X, class S = X>
		inline X pdf(X x, S s = 0)
		{
			static X sqrt2pi = std::sqrt(2 * std::numbers::pi_v<X>);
			X x_s = x - s;

			return std::exp(-x_s * x_s / 2) / sqrt2pi;
		}

		// standard normal share cumulative distribution function
		template<class X, class S = X>
		inline X cdf(X x, S s = 0)
		{
			static X sqrt2 = std::numbers::sqrt2_v<X>;

			return 0.5 * (1 + std::erf((x - s) / sqrt2));
		}

	} // namespace normal

	// F <= k if and only if Z <= moneyness(f,s,k)
	template<class F, class S, class K>
	inline auto moneyness(F f, S s, K k)
	{
		if (f <= 0 or k <= 0 or s <= 0)
			return std::numeric_limits<F>::quiet_NaN();

		return std::log(k / f) / s + s / 2;
	}

	namespace put {

		template<class F, class S, class K>
		inline auto value(F f, S s, K k)
		{
			if (s == 0) {
				return std::max(k - f, 0);
			}
			auto m = moneyness(f, s, k);

			return k * normal::cdf(m) - f * normal::cdf(m, s);
		}
		template<class F, class S, class K>
		inline auto delta(F f, S s, K k)
		{
			auto m = moneyness(f, s, k);

			return -normal::cdf(m, s);
		}
		template<class F, class S, class K>
		inline auto gamma(F f, S s, K k)
		{
			auto m = moneyness(f, s, k);

			return normal::pdf(m, s)/(f*s);
		}
		template<class F, class S, class K>
		inline auto vega(F f, S s, K k)
		{
			auto m = moneyness(f, s, k);

			return k * normal::pdf(m);
		}

		// return s with p = value(f, s, k)
		template<class F, class P, class K>
		inline auto implied(F f, P p, K k, P s0 = 0.1)
		{
			const auto v = [=](P s) { return value(f, s, k) - p; };
			const auto dv = [=](P s) { return vega(f, s, k); };

			return root1d::newton::solve(v, dv, s0);
		}

	} // namespace put
	
	namespace call {

		template<class F, class S, class K>
		inline auto value(F f, S s, K k)
		{
			return f - k + put::value(f, s, k);
		}
		template<class F, class S, class K>
		inline auto delta(F f, S s, K k)
		{
			return -put::delta(f, s, k);
		}
		template<class F, class S, class K>
		inline auto gamma(F f, S s, K k)
		{
			return put::gamma(f, s, k);
		}
		template<class F, class S, class K>
		inline auto vega(F f, S s, K k)
		{
			return put::vega(f, s, k);
		}
		// return s with c = call::value(f, s, k)
		template<class F, class C, class K>
		inline auto implied(F f, C c, K k, C s0 = 0.1)
		{
			return put::implied(f, c - f + k, k, s0);
		}

	} // namespace call

} // namespace tmx::black
