// tmx_ho_lee.h - Ho-Lee model with constant volatility.
// f_t = r(t) + σ * B_t
// D_t = exp(-int_0^t f_s ds) 
//     = exp(-int_0^t r(s) ds - σ int_0^t B_s ds)
//   E[exp(N)] = exp(E[N] + Var(N)/2)
// D(t) = E[D_t] = exp(-int_0^t r(s) ds + σ^2 t^3/6)
// f(t) = r(t) - σ^2 t^2/2
// 
//   int_t^u B_s ds = int_t^u (u - s) dB_s + (u - t) B_t
// 
// D_u/D_t = exp(-int_t^u f_s ds) 
//         = exp(-int_t^u r(s) ds - σ int_t^u (u - s) dB_s - σ (u - t) B_t)
// 
//   E_t[exp(-σ int_t^u (u - s) dB_s)] = exp(σ^2 int_t^u (u - s)^2/2 ds)
//                                     = exp(σ^2 (u - t)^3/6)
// D_t(u) = E_t[D_u/D_t]
//        = exp(-int_t^u r(s) ds + σ^2 (u - t)^3/6 - σ (u - t) B_t)
//              f_t(u) = r(u) - σ^2(u - t)^2/2 - σ B_t
//        = D(u)/D(t) exp(-σ^2 u^3/6 σ^2 + t^3/6 + σ^2 (u - t)^3/6 - σ (u - t) B_t)
//        = D(u)/D(t) exp(-σ^2 ut(u - t)/2 - σ (u - t) B_t)
// E[D_t(u)] = D(u)/D(t) exp(-σ^2 ut(u - t)/2 + σ^2 (u - t)^2 t/2)
//           = D(u)/D(t) exp(σ^2 t(u - t)(-u + u -  t)/2)
//           = D(u)/D(t) exp(-σ^2 t^2(u - t)/2)
#pragma once
#include <cmath>
#include "tmx_instrument.h"
#include "tmx_pwflat_value.h"

namespace tmx::ho_lee {

	// E[D_t(u)]
	template<class X = double>
	inline auto ED(X Dt, X Du, X t, X u, X σ) 
	{
		return (Du/Dt) * std::exp(-σ * σ * t * t * (u - t) / 2);
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
		// Expected value at time t
		template<class U = double, class C = double>
		auto value(const instrument<U,C>& i, T t = 0) const
		{
			auto v = 0;

			auto Dt = f.discount(t);
			for (size_t j = 0; j < m and u[j] >= t; ++j) {
				v += c_[j] * ED(Dt, f.discount(u_[j]), t, u_[j], σ);
			}

			return v;
		}
	};

}
