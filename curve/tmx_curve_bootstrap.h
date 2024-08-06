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
		// fix up initial guess
		if (std::isnan(_f)) {
			_f = f(_t); // last forward rate
		}
		if (std::isnan(_f)) {
			_f = 0.01;
		}

		const auto vp = [i, &f, _t, p](F f_) { return valuation::present(i, extrapolate(f, _t, f_)) - p; };
		//const auto vd = [i, &f, _t](F f_) { return valuation::duration(i, extrapolate(f, _t, f_)); };
		double pv1, pv2;
		pv1 = vp(.01);
		pv2 = vp(.02);
		
		auto [f_, tol, n] = root1d::secant(_f, _f + .01).solve(vp);
		_f = f_;

		return { _t, _f };
	}
#ifdef _DEBUG
	inline int bootstrap_test()
	{
		using namespace fms::iterable;
		{
			curve::constant<> f;
			double r = 0.1;
			auto zcb = instrument::zero_coupon_bond(1, std::exp(r));
			auto D = extrapolate(f, 0., r).discount(1);
			assert(D < 1);
			auto p = valuation::present(zcb, extrapolate(f, 0., r)); 
			assert(p == 1);
			auto d = valuation::duration(zcb, extrapolate(f, 0., r));
			assert(d == -1);
			auto [_t, _f] = curve::bootstrap(zcb, f, 0., 0.2, 1.);
			assert(_t == 0);
			assert(std::fabs(_f - r) <= math::sqrt_epsilon<double>);
		}
		{
			double u[] = { 1, 2, 3 };
			double c[] = { 0.01, 0.02, 0.03 };

			curve::pwflat<> f;
			auto i1 = instrument::iterable(take(array(u), 1), take(array(c), 1));
			auto uc1 = bootstrap(i1, f, 0., .01, valuation::present(i1, constant(0.02)));
			f.push_back(uc1);

			auto i2 = instrument::iterable(take(array(u), 2), take(array(c), 2));
			auto uc2 = bootstrap(i1, f, 1., .01, valuation::present(i1, constant(0.02)));
			f.push_back(uc2);

			auto i3 = instrument::iterable(take(array(u), 3), take(array(c), 3));
			auto uc3 = bootstrap(i1, f, 2., .01, valuation::present(i1, constant(0.02)));
			f.push_back(uc3);
			double f0;
			f0 = f(1);
			f0 = f(2);
			f0 = f(3);
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
