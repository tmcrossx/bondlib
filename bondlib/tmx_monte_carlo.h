// tmx_monte_carlo.h - Monte Carlo simulation
#pragma once
//#include <random>
#include "tmx_monoid.h"

namespace tmx {

	template<class F, class X>
	inline X monte_carlo(F f, const monoid_base<X>& m, size_t n = 1000)
	{
		X v = m.id();

		for (n--) {
			v = m.op(v, f());
		}

		return v;
	}

} // namespace tmx
