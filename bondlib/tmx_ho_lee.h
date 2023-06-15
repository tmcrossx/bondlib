// tmx_ho_lee.h - Ho-Lee model with constant volatility.
// f_t = r(t) + σ * B_t
// D_t = exp(-int_0^t f_s ds) = exp(-int_0^t r(s) ds - σ int_0^t B_s ds)
// D(t) = E[D_t] = exp(-int_0^t r(s) ds + σ^2 t^3/6)
// f(t) = r(t) - σ^2 t^2/2
// int_t^ u B_s ds = (u - t) B_t + int_t ^ u(u - s) dB_s
// 
// D_u/D_t = exp(-int_t^u f_s ds) = exp(-int_t^u r(s) ds - σ (u - t) B_t - σ int_t^u (u - s) dB_s)
// D_t(u) = E_t[D_u/D_t]
//        = exp(-int_t^u r(s) ds + σ^2 (u - t)^3/6 - σ (u - t) B_t)
//              f_t(u) = r(u) - σ^2(u - t)^2/2 + σ B_t
//        = D(u)/D(t) exp(-σ^2 u^3/6 σ^2 + t^3/6 + σ^2 (u - t)^3/6 - σ (u - t) B_t)
//        = D(u)/D(t) exp(-σ^2 ut(u - t)/2 - σ (u - t) B_t)
// E[D_t(u)] = D(u)/D(t) exp(-σ^2 ut(u - t)/2 + σ^2 (u - t)^2 t/2)
//           = D(u)/D(t) exp(σ^2 t(u - t)(-u + u -  t)/2)
//           = D(u)/D(t) exp(-σ^2 t^2(u - t)/2)
#pragma once
#include <cmath>
#include "tmx_pwflat.h"

namespace tmx::ho_lee {

	// E[D_t(u)]
	template<class X = double>
	constexpr X ED(X Dt, X Du, X t, X u, X σ) 
	{
		return (Du/Dt) * std::exp(-σ * σ * t * t * (u - t) / 2);
	}

	template<class U = double, class C = double, class X = double>
	constexpr X value(pwflat<U,C>::curve_view, X Dt, X Du, X t, X u, X σ)
	{
		return std::exp(-σ * σ * u * u * u / 6 + c);
	}

}
