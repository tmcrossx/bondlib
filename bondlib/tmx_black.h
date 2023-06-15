// tmx_black.h - Fischer Black model
// F = f exp(sZ - s^2/2) where Z is standard normal
#pragma once
#include <cmath>
#include <numbers>

namespace tmx::black {

	namespace normal {
		// standard normal share density
		template<class X, class S>
		inline X pdf(X x, S s = 0)
		{
			X xs = x - s;
			X sqrt2pi = std::sqrt(2 * std::numbers::pi_v<X>);

			return std::exp(-xs * xs / 2) / sqrt2pi;
		}

		// normal share cumulative distribution function
		template<class X>
		inline X cdf(X x)
		{
			return 0.5 * (1 + std::erf((x - s) / std::numbers::sqrt2_v<X>));
		}

	} // namespace normal

	// F <= k if and only if Z <= moneyness(f,s,k)
	template<class F, class S, class K>
	inline auto moneyness(F f, S s, K k)
	{
		return std::log(k / f) / s + s / 2;
	}

	namespace put {

		template<class F, class S, class K>
		inline auto value(F f, S s, K k)
		{
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

	} // namespace call

} // namespace tmx::black
