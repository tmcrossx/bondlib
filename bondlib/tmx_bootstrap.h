// tmx_bootstrap.h - Bootstrap a pwflat forward curve.
#pragma once
#include "tmx_root1d.h"
#include "tmx_value.h"

namespace tmx::bootstrap {

	template<class X>
	constexpr X NaN = std::numeric_limits<X>::quiet_NaN();

	// bootstrap single instrument
	template<class U = double, class C = double, class T = double, class F = double>
	constexpr std::pair<T, F> instrument(
		size_t m, const U* u, const C* c, 
		size_t n, const T* t, const F* f, F p = 0)
	{
		if (m == 0) {
			return { NaN<T>, NaN<F> };
		}
		U _u = u[m - 1];
		F _f = n == 0 ? 0 : f[n - 1];
		T _t = n == 0 ? 0 : t[n - 1];
		if (_u <= _t) {
			return { NaN<T>, NaN<F> };
		}

		_f = secant::solve([m, u, c, n, t, f, p](F f0) { 
			return value::present(m, u, c, n, t, f, f0) - p; }, _f, _f + 0.01);

		return { _u, _f };
	}

}
