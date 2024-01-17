﻿// tmx_black.h - Fischer Black model
// F = f exp(sX - κ(s)) where X has mean 0, variance 1, and κ(s) = log E[exp(s X)].
// Note E[F] = f and Var(log F) = s^2.
#pragma once
#include <cmath>
#include <numbers>
#include "tmx_variate_normal.h"
#include "tmx_root1d.h"

namespace tmx::black {

	// F <= k if and only if X <= (log(k/f) + κ(s))/s
	template<class F, class S, class K, class V>
	inline auto moneyness(F f, S s, K k, const V& v = variate::normal{})
	{
		if (f <= 0 or k <= 0 or s <= 0) {
			return math::NaN<F>;
		}

		return (std::log(k / f) + v.cgf(s))/s;
	}

	namespace put {

		template<class F, class S, class K, class V>
		inline auto value(F f, S s, K k, const V& v = variate::normal{})
		{
			if (f == 0 or k == 0) {
				return F(0);
			}
			if (s == 0) {
				return std::max(k - f, F(0));
			}

			auto x = moneyness(f, s, k, v);

			return k * v.cdf(x) - f * v.cdf(x, s);
		}

		template<class F, class S, class K, class V>
		inline auto delta(F f, S s, K k, const V& v = variate::normal{})
		{
			if (f == 0 or k == 0) {
				return F(0);
			}
			if (s == 0) {
				return F(-1)*(f <= k);
			}

			auto x = moneyness(f, s, k, v);

			return -v.cdf(x, s);
		}

		template<class F, class S, class K, class V>
		inline auto gamma(F f, S s, K k, const V& v = variate::normal{})
		{
			if (f == 0 or k == 0) {
				return X(0);
			}
			if (s == 0) {
				return math::infinity<F> *(f == k);
			}

			auto x = moneyness(f, s, k, v);

			return v.pdf(x, s)/(f*s);
		}
		template<class F, class S, class K, class V>
		inline auto vega(F f, S s, K k, const V& v = variate::normal{})
		{
			if (f == 0 or k == 0) {
				return F(0);
			}
			if (s == 0) {
				return k == f ? k * v.pdf(F(0)) : F(0);
			}
			auto x = moneyness(f, s, k, v);

			return k * v.pdf(x);
		}

		// Return Black implied vol s with p = value(f, s, k)
		template<class F, class P, class K>
		inline auto implied(F f, P p, K k, P s0 = 0.1,
			double tol = math::sqrt_epsilon<P>, int iter = 100)
		{
			const auto v = [=](P s) { return value(f, s, k) - p; };
			const auto dv = [=](P s) { return vega(f, s, k); };

			return root1d::newton(s0, tol, iter)::solve(v, dv);
		}

	} // namespace put
	
	namespace call {

		template<class F, class S, class K, class V>
		inline auto value(F f, S s, K k, const V& v = variate::normal{})
		{
			return f - k + put::value(f, s, k, v);
		}

		template<class F, class S, class K, class V>
		inline auto delta(F f, S s, K k, const V& v = variate::normal{})
		{
			return -put::delta(f, s, k, v);
		}

		template<class F, class S, class K, class V>
		inline auto gamma(F f, S s, K k, const V& v = variate::normal{})
		{
			return put::gamma(f, s, k, v);
		}

		template<class F, class S, class K, class V>
		inline auto vega(F f, S s, K k, const V& v = variate::normal{})
		{
			return put::vega(f, s, k, v);
		}

		// return s with c = call::value(f, s, k)
		template<class F, class C, class K>
		inline auto implied(F f, C c, K k, C s0 = 0.1,
			double tol = root1d::sqrt_epsilon<C>, int iter = 100)
		{
			return put::implied(f, c - f + k, k, s0);
		}

	} // namespace call

} // namespace tmx::black
