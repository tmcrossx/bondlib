// tmx_curve_bootstrap.h - Bootstrap a piecewise-flat curve forward curve.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <utility>
#include "instrument/tmx_instrument.h"
#include "curve/tmx_curve_pwflat.h"
#include "valuation/tmx_valuation.h"
#include "math/tmx_math_limits.h"

namespace tmx::curve {

	// bootstrap1 - cash deposit
	// bootstrap2 - forward rate agreement

	// Bootstrap a single instrument given last time on curve and optional initial forward rate guess.
	// Return point on the curve repricing the instrument.
	template<class IU, class IC, class T = double, class F = double>
	inline std::pair<T, F> bootstrap0(instrument::iterable<IU, IC> i, const curve::interface<T, F>& f,
		T _t, F _f = math::NaN<F>, F p = 0)
	{
		const auto uc = i.last(); // last instrument cash flow
		if (uc.u <= _t) {
			return { math::NaN<T>, math::NaN<F> };
		}	

		// fix up initial guess
		if (std::isnan(_f)) {
			_f = f(_t); // last forward rate
		}
		if (std::isnan(_f)) {
			_f = 0.01;
		}

		const auto vp = [i, &f, _t, p](F f_) { return valuation::present(i, extrapolate(f, _t, f_)) - p; };
		
		auto [f_, tol, n] = root1d::secant(_f, _f + 0.01).solve(vp);
		_f = f_;

		return { uc.u, _f};
	}
	// Bootstrap a piecewise flat curve from instruments and prices.
	template<class I, class P>
	constexpr auto bootstrap(I is, P ps, double _t = 0, double _f = 0.03)
	{
		curve::pwflat<> f;
		
		while (is and ps) {
			std::tie(_t, _f) = bootstrap0(*is, f, _t, _f, *ps);
			f.push_back(_t, _f);
			++is;
			++ps;
		}

		return f;
	}
#ifdef _DEBUG
	inline int bootstrap_test()
	{
		using namespace fms::iterable;
		{
			curve::constant<> f;
			double r = 0.1;
			auto zcb = instrument::zero_coupon_bond(1, std::exp(r));
			auto D = f.discount(1, 0., r);
			assert(D < 1);
			auto p = valuation::present(zcb, extrapolate(f, 0., r));
			assert(p == 1);
			auto d = valuation::duration(zcb, extrapolate(f, 0., r));
			assert(d == -1);
			auto [_t, _f] = curve::bootstrap0(zcb, f, 0., 0.2, 1.);
			assert(_t == 0);
			assert(std::fabs(_f - r) <= math::sqrt_epsilon<double>);
		}

		return 0;
	}
#endif // _DEBUG

}
