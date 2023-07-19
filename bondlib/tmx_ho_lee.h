// tmx_ho_lee.h - Ho-Lee model with constant volatility.
// D_t = exp(-int_0^t φ(s) ds - int_0^t σ(t - s) dB_s)
//     ~ exp(-int_0^t φ(s) ds - B_t σ t/sqrt(3))
//     = D(t) exp(-σ^2 t^3/6 - B_t σ t/sqrt(3))
// Var(int_0^t σ(t - s) dB_s) = σ^2 int_0^t (t - s)^2 ds = σ^2 t^3/3
// D(t) = E[D_t] = exp(-int_0^t φ(s) ds + σ^2 t^3/6)
// D_t(u) = exp(-int_t^u φ(s) ds + σ^2 (u - t)^3/6 - σ (u - t) B_t)
//        = D(u)/D(t) exp(σ^2 [-u^3 + t^3 + (u - t)^3]/6 - σ (u - t) B_t)
//        = D(u)/D(t) exp(σ^2 [-3u^2 t + 3 u t^2]/6 - σ (u - t) B_t)
// E[log D_t(u)] = log(D(u)/D(t)) - σ^2 ut (u - t)/2
// Var(log D_t(u)) = σ^2 (u - t)^2 t
#pragma once
#include <cmath>
#include <utility>
#include "tmx_black.h"
#include "tmx_curve.h"
#include "tmx_instrument.h"

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
	// D(t) = E[D_t]
	template<class X = double>
	inline auto ED(X t, X φ, X σ)
	{
		return std::exp(ELogD(t, φ) + VarLogD(t, σ) / 2);
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
		return std::exp(ELogD(Dt, Du, t, u, σ) + VarLogD(t, u, σ) / 2);
	}

	// D_t(u) D_t
	// E[e^N e^M] = E[e^N] E[e^M] exp(Cov(N,M))
	// E[D_t(u) D_t] = E[D_t(u)] D(t) exp(Cov(-σ(u - t)B_t, -int_0^t σB_s ds)]
	template<class X = double>
	inline auto CovLogD_(X t, X u, X σ)
	{
		return σ * σ * (u - t) * t * t / 2
	}
	template<class X = double>
	inline auto ELogD_(X Dt, X Du, X t, X u, X σ)
	{
		return ED(Dt, Du, t, u, σ) * Dt * std::exp(σ * σ * (u - t) * t * t / 2);
	}



	// E[e^N e^M] = E[e^N] E[e^M] exp(Cov(N,M))
	// E[D_t(u) D_t] = E[D_t(u)] D(t) exp(Cov(-σ(u - t)B_t, -int_0^t σB_s ds)]
	template<class X = double>
	inline auto CovD(X Dt, X Du, X t, X u, X σ)
	{
		return ED(Dt, Du, t, u, σ) * Dt * std::exp(σ * σ * (u - t) * t * t / 2);
	}
	// E[e^N e^M] = E[e^N] E[e^M] exp(Cov(N,M))
	// E[D_t(u) D_t]/D(t) = E[D_t(u)] exp(Cov(-σ(u - t)B_t, -int_0^t σB_s ds)]
	template<class X = double>
	inline auto ED_(X Dt, X Du, X t, X u, X σ)
	{
		return ED(Dt, Du, t, u, σ) * std::exp(σ * σ * (u - t) * t * t / 2);
	}
	// Cov(e^N, e^M) = exp(Cov(N,M))
	// Covariance of D_t(u) and D_t(v)
	template<class X = double>
	inline auto Cov(X Dt, X Du, X Dv, X t, X u, X v, X σ)
	{
		auto Dtu = ED(Dt, Du, t, u, σ);
		auto Dtv = ED(Dt, Dv, t, v, σ);
		auto cov = σ * σ * (u - t) * (v - t) * t;

		return Dtv * Dtu * std::exp(cov);
	}

	// E_t[sum_{u_j > t} c_j D_t(u_j) D_t]
	template<class U, class C, class T, class F>
	inline auto value(const instrument<U, C>& i, const curve<T, F>& f, T t, F σ)
	{
		F mean = 0, mean_ = 0, var = 0;
	}

	// mean and log variance of P_t = sum_{u_j > t} c_j D_t(u_j) and E[P_t D_t]/D(t)
	template<class U, class C, class T, class F>
	inline std::tuple<F, F, F> moments(const instrument<U, C>& i, const curve<T, F>& f, T t, F σ)
	{
		F mean = 0, mean_ = 0, var = 0;

		const auto u = i.time();
		auto j = u.offset(t);
		auto m = static_cast<decltype(j)>(u.size());
		const auto c = i.cash();

		auto Dt = f.discount(t);
		for (auto k = j; k < m; ++k) {
			auto Duk = f.discount(u[k]);
			mean += c[k] * ED(Dt, Duk, t, u[k], σ);
			mean_ += c[k] * ED_(Dt, Duk, t, u[k], σ);
			for (auto l = j; l < m; ++l) {
				auto Dul = f.discount(u[l]);
				var += c[k] * c[l] * Cov(Dt, Duk, Dul, t, u[l], u[k], σ);
			}
		}
		// Var(e^N) = E[e^N]^2 (e^Var(N) - 1)
		var = std::log(var / (mean * mean) + 1);

		return { mean, mean_, var };
	}
	/*
	// Cov(sum_{u_j > t} c_j D_t(u_j), D_t)
	template<class U, class C, class T, class F>
	inline F Cov(const instrument<U, C>& i, const curve<T, F>& f, T t, F σ)
	{
	}
	*/

	// E[f(B_t) e^N] = E[f(B_t + Cov(B_t, N))] E[e^N]
	// E[(P_t - p)^+ D_t] 
	// = E[(sum_{u_j > t} c_j D_t(u_j) - p)^+ D_t]
	// = E[(sum_{u_j > t} c_j D_t(u_j) exp(σ^2(u_j - t) - p)^+] D(t)
	template<class U, class C, class T, class F>
	inline F option(const instrument<U, C>& i, const curve<T, F>& f, T t, F σ, F p)
	{
		auto [m, m_, v] = moments(i, f, t, σ);
		// σ += cov!!!
		return black::call::value(m_, std::sqrt(v), p) * f.discount(t);
	}

}
