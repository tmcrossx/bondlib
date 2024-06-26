// tmx_bootstrap.h - Bootstrap a piecewise-flat curve forward curve.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <utility>
#include "../instrument/tmx_instrument.h"
#include "../curve/tmx_curve_pwflat.h"
#include "../valuation/tmx_valuation.h"
#include "../math/tmx_math_limits.h"

namespace tmx::bootstrap {

	// instrument1 - cash deposit
	// instrument2 - forward rate agreement

	// Bootstrap a single instrument.
	// Return point on the curve repricing the instrument.
	template<class I, class T = double, class F = double>
	inline std::pair<T, F> instrument(I i, curve::pwflat<T,F>& f, 
		F p = 0, F _f = math::NaN<F>)
	{
		const auto _u = back(i).u;
		const auto [t_, f_] = f.back();
		if (_u <= t_) { // maturity before last point on curve
			return { math::NaN<T>, math::NaN<F> };
		}

		// fix up initial guess
		if (std::isnan(_f)) {
			_f = f_; // last forward rate
		}
		if (std::isnan(_f)) {
			_f = 0.01;
		}

		const auto vp = [&](F f0) { return valuation::present(i, f.extrapolate(f0)) - p; };
		const auto vd = [=](F f0) { return valuation::duration(i, f.extrapolate(f0)); };
		
		_f = root1d::newton(_f).solve(vp, vd);

		return { _u, _f };
	}
#ifdef _DEBUG
	inline int instrument_test()
	{
		{
			//curve::constant<> f;
			//double r = 0.1;
			//const auto i = instrument::iterable({ 1. }, { std::exp(r) });
			//auto [_t, _f] = tmx::bootstrap::instrument(i, f, 1.);
			//assert(_t == 1);
			//assert(std::fabs(_f - r) <= math::sqrt_epsilon<double>);
		}

		return 0;
	}
#endif // _DEBUG
	/*
	template<class U = double, class C = double, class T = double, class F = double>
	constexpr curve::curve<T, F> instruments(iterable<instrument<U,C>> is)
	{
		curve::curve<T, F> f;
		
		while (is) {
			f.push_back(instrument(*is, f));
			++is;
		}

		return f;
	}
	*/
}
