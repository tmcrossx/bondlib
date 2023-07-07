﻿// tmx_ho_lee.h - Ho-Lee model with constant volatility.
// D_t = exp(-int_0^t φ(s) ds - int_0^t σ(t - s) dB_s)
// Var(int_0^t σ(t - s) dB_s) = σ^2 int_0^t (t - s)^2 ds = σ^2 t^3/3
// D(t) = exp(-int_0^t φ(s) ds + σ^2 t^3/6)
// D_t(u) = exp(-int_t^u φ(s) ds + σ^2 (u - t)^3/6 - σ (u - t) B_t)
//        = D(u)/D(t) exp(σ^2 [-u^3 + t^3 + (u - t)^3]/6 - σ (u - t) B_t)
//        = D(u)/D(t) exp(σ^2 [-3u^2 t + 3 u t^2]/6 - σ (u - t) B_t)
// E[log D_t(u)] = log(D(u)/D(t)) - σ^2 ut (u - t)/2
// Var(log D_t(u)) = σ^2 (u - t)^2 t
#pragma once
#include <cmath>
#include <utility>
#include "tmx_instrument.h"
#include "tmx_pwflat.h"

namespace tmx::ho_lee {

	// E[log D_t]
	template<class X = double>
	inline auto ELogD(X t, X φ)
	{
		return -φ * t;
	}
	// Var(log D_t)
	template<class X = double>
	inline auto VarLogD(X t, X σ)
	{
		return σ * σ * t * t * t / 3;
	}
	template<class X = double>
	inline auto ED(X t, X φ, X σ)
	{
		return std::exp(ELogD(t, φ) + VarLogD(t, σ)/2);
	}


	// E[log D_t(u)]
	template<class X = double>
	inline auto ELogD(X Dt, X Du, X t, X u, X σ)
	{
		return std::log(Du / Dt) - σ * σ * u * t * (u - t) / 2;
	}
	// Var[log D_t(u)]
	template<class X = double>
	inline auto VarLogD(X t, X u, X σ)
	{
		return σ * σ * (u - t) * (u - t) * t;
	}
	// E[D_t(u)]
	template<class X = double>
	inline auto ED(X Dt, X Du, X t, X u, X σ)
	{
		return std::exp(ELogD(Dt, Du, t, u, σ) + VarLogD(t, u, σ)/2);
	}

	// Approximate sum of log-normal random variables by a single log-normal.
	// E[sum_j exp(N_j)] = sum E[exp(N_j)].
	// Var(sum_j exp(N_j)) = sum_j,k Cov(exp(N_j), exp(N_k))
	// Cov(exp(N), exp(M)) = E[exp(N)] E[exp(M)] (exp(Cov(N, M)) - 1)
	/*
	template<class X>
	inline std::pair<X,X> log_normal_sum(size_t n, const X* μ, const X* σ)
	{
	}
	*/

	/*
	// E[sum c_j D_t(u_j)]
	template<class U = double, class C = double, class X = double>
	constexpr X value(size_t m, const U* u_, const C* c_, X Dt, X t, X σ)
	{
		X v = 0;

		for (size_t j = 0; j < m; ++j) {
			v += c_[j] * ED(Dt, Du, t, u_[j], σ);
		}

		return v;
	}
	*/
	template<class T = double, class F = double, class S = double>
	class model {
		pwflat::curve_value<T, F> f; // forward curve
		S σ;
	public:
		model(const pwflat::curve_value<T, F>& f, S σ)
			: f(f), σ(σ)
		{ }
		/*
		// Expected value at time t
		template<class U = double, class C = double>
		auto value(const instrument<U,C>& i, T t = 0) const
		{
			auto v = 0;

			auto Dt = f.discount(t);
			for (size_t j = 0; j < m and u[j] >= t; ++j) {
				v += c[j] * ED(Dt, f.discount(u_[j]), t, u_[j], σ);
			}

			return v;
		}
		*/
	};

}
