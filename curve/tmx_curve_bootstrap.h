// tmx_curve_bootstrap.h - Bootstrap a piecewise-flat curve forward curve.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <utility>
#include "instrument/tmx_instrument.h"
#include "curve/tmx_curve.h"
#include "valuation/tmx_valuation.h"
#include "math/tmx_math_limits.h"

namespace tmx::curve {

	// bootstrap1 - cash deposit
	// bootstrap2 - forward rate agreement

	// Bootstrap a single instrument given last time on curve and optional initial forward rate guess.
	// Return point on the curve repricing the instrument.
	template<class IU, class IC, class T = double, class F = double>
	inline std::pair<T, F> bootstrap(instrument::iterable<IU, IC> i, const curve::interface<T, F>& f, 
		T _t, F _f = math::NaN<F>, F p = 0)
	{
		auto [_u, _c] = *last(i); // last cash flow time
		// if (_u <= _t) ...
		// fix up initial guess
		if (std::isnan(_f)) {
			_f = f(_t); // last forward rate
		}
		if (std::isnan(_f)) {
			_f = 0.01;
		}

		const auto vp = [i, &f, _t, p](F f_) { return valuation::present(i, extrapolate(f, _t, f_)) - p; };
		const auto vd = [i, &f, _t](F f_) { return valuation::duration(i, extrapolate(f, _t, f_)); };
		
		_f = root1d::newton(_f).solve(vp, vd);

		return { _u, _f };
	}
#ifdef _DEBUG
	inline int bootstrap_test()
	{
		using namespace fms::iterable;
		{
			curve::constant<> f;
			double r = 0.1;
			auto u0 = single(1.);
			auto c0 = single(std::exp(r));

			auto [_t, _f] = curve::bootstrap(instrument::iterable(u0, c0), f, 0., 0.2, 1.);
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
