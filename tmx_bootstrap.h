// tmx_bootstrap.h - Bootstrap a piecewise-flat curve forward curve.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include "tmx_instrument.h"
#include "tmx_curve_pwflat.h"
#include "tmx_valuation.h"
#include "tmx_math.h"

namespace tmx::bootstrap {

	// instrument1 - cash deposit
	// instrument2 - forward rate agreement

	// Bootstrap a single instrument.
	// Return point on the curve repricing the instrument.
	template<fms::iterable::input I, class T = double, class F = double>
	inline std::pair<T, F> instrument(I i, curve::pwflat<T,F>& f, 
		F p = 0, F _f = math::NaN<F>)
	{
		const auto [_u, _c] = back(i).pair();

		const auto [_t, f_] = f.back();
		if (std::isnan(f_)) { // empty curve
			if (std::isnan(_f)) {
				_f = 0.01;
			}
		}
		else {
			assert(_u > _t);
			if (std::isnan(_f)) {
				_f = f_; // use last rate
			}
		}

		//const instrument::interface<U, C>& ii = i.clone();
		const auto vp = [&](F f0) { return valuation::present(i, f.extrapolate(f0)) - p; };
		const auto vd = [=](F f0) { return f0/*value::duration(i, f.extrapolate(f0)) */ ; };
		
		_f = root1d::newton(_f).solve(vp, vd);

		return { _u, _f };
	}
#ifdef _DEBUG
	inline int instrument_test()
	{
		{
			curve::constant<> f;
			double r = 0.1;
			auto [_t, _f] = bootstrap::instrument(instrument::zero_coupon_bond<>(1., std::exp(r)), f, 1.);
			assert(_t == 1);
			assert(std::fabs(_f - r) <= math::sqrt_epsilon<double>);
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
