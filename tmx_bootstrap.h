// tmx_bootstrap.h - Bootstrap a curve forward curve.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include "ensure.h"
#include "tmx_value.h"

namespace tmx::bootstrap {

	// Bootstrap a single instrument.
	// Return point on the curve repricing the instrument.
	template<class U = double, class C = double, class T = double, class F = double>
	constexpr std::pair<T, F> instrument(const instrument::base<U,C>& i, curve::base<T,F>& f, 
		F p = 0, F _f = NaN<F>)
	{
		ensure (i.size() != 0);
		const auto [_u, _c] = i.back();

		const auto [_t, f_] = f.back();
		if (std::isnan(f_)) { // empty curve
			if (std::isnan(_f)) {
				_f = 0.01;
			}
		}
		else {
			ensure(_u > _t);
			if (std::isnan(_f)) {
				_f = f_; // use last rate
			}
		}

		const auto vp = [&](F f0) { return value::present(i, f.extrapolate(f0)) - p; };
		const auto vd = [&](F f0) { return value::duration(i, f.extrapolate(f0)); };
		
		_f = root1d::newton::solve(vp, vd, _f);

		return { _u, _f };
	}
#ifdef _DEBUG
	inline int instrument_test()
	{
		{
			curve::constant<> f;
			double r = 0.1;
			auto [_t, _f] = bootstrap::instrument(instrument::zero_coupon_bond<>(1, std::exp(r)), f, 1.);
			assert(_t == 1);
			assert(std::fabs(_f - r) <= root1d::sqrt_epsilon<double>);
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
