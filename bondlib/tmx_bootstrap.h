// tmx_bootstrap.h - Bootstrap a pwflat forward curve.
#pragma once
#include "tmx_value.h"

namespace tmx::bootstrap {

	template<class X>
	constexpr X NaN = std::numeric_limits<X>::quiet_NaN();

	// Bootstrap a single instrument.
	// Return point on the curve repricing the instrument.
	template<class U = double, class C = double, class T = double, class F = double>
	constexpr std::pair<T, F> instrument(const tmx::instrument<U,C>& i, pwflat::curve_view<T,F> f, 
		F p = 0, F _f = NaN<F>)
	{
		ensure (i.size() != 0);
		const auto [_u, _c] = i.back();

		if (f.size() == 0) {
			if (std::isnan(_f)) {
				_f = 0.01;
			}

			_f = secant::solve([&](F f0) {
				return value::present(i, pwflat::curve_view(f0)) - p; }, _f, _f + 0.01);
		}
		else {
			const auto [_t, f_] = f.back();
			ensure(_u > _t);
			if (std::isnan(_f)) {
				_f = f_;
			}
			_f = secant::solve([&](F f0) {
				return value::present(i, f.extrapolate(f0)) - p; }, _f, _f + 0.01);
		}

		return { _u, _f };
	}
	/*
	template<class U = double, class C = double, class T = double, class F = double>
	constexpr pwflat::curve_value<T, F> instruments(iterable<instrument<U,C>> is)
	{
		pwflat::curve_value<T, F> f;
		
		while (is) {
			f.push_back(instrument(*is, f));
			++is;
		}

		return f;
	}
	*/
}
