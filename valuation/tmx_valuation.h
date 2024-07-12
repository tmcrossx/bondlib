// tmx_valuation.h - present value, duration, convexity, yield, oas
#pragma once
#include <cmath>
#include "curve/tmx_curve.h"
#include "instrument/tmx_instrument.h"
#include "math/tmx_root1d.h"

using namespace fms::iterable;

namespace tmx::valuation {

	// Convert between continuous rate and compounded yield using (1 + y/n)^n = e^r
	template<class X>
	inline X continuous_rate(X y, unsigned n)
	{
		return X(std::log(std::pow(1 + y / n, n)));
	}
	template<class X>
	inline X compound_yield(X r, unsigned n)
	{
		return X(n * std::expm1(r / n));
	}

	// Present value at t of a zero coupon bond with cash flow c at time u.
	template<class U, class C, class T, class F>
	constexpr C present(const cash_flow<U, C>& uc, const curve::interface<T, F>& f) 
	{
		return uc.c * f.discount(uc.u);
	}
#ifdef _DEBUG
	//static_assert(present<int,int,int,int>(instrument::cash_flow(1, 1), curve::constant(0)) == 1);
#endif // _DEBUG

	// Present value at of discounted cash flows.
	template<class IU, class IC, class T, class F>
	auto present(instrument::iterable<IU,IC> i, const curve::interface<T, F>& f)
	{
		return sum(apply([&f](const auto& uc) { return present(uc, f); }, i));
	}
#ifdef _DEBUG
	//static_assert(present<int,int,int,int>(instrument::zero_coupon_bond(1, 2), curve::constant(0)) == 2);
#endif // _DEBUG

	// TODO: risky_present(i, f, T, R) ...

	// Derivative of present value with respect to a parallel shift.
	template<class IU, class IC, class T, class F>
	constexpr auto duration(instrument::iterable<IU, IC> i, const curve::interface<T, F>& f)
	{
		return sum(apply([&f](const auto& uc) { return -(uc.u) * present(uc, f); }, i));
	}

	// Duration divided by present value.
	template<class IU, class IC, class T, class F>
	constexpr auto macaulay_duration(instrument::iterable<IU, IC> i, const curve::interface<T, F>& f)
	{
		return duration(i, f)/ present(i, f);
	}

	// Second derivative of present value with respect to a parallel shift.
	template<class IU, class IC, class T, class F>
	constexpr auto convexity(instrument::iterable<IU, IC> i, const curve::interface<T, F>& f)
	{
		return sum(apply([&f](const auto& uc) { return uc.u * uc.u * present(uc, f); }, i));
	}

	// Constant yield matching price p.
	template<class IU, class IC, 
		class C = typename IC::value_type, class U = typename IU::value_type>
	inline C yield(instrument::iterable<IU, IC> i, C p = 0,
		C y0 = 0.01, C tol = math::sqrt_epsilon<C>, int iter = 100)
	{
		const auto pv = [p,&i](C y_) { return present(i, curve::constant<U, C>(y_)) - p; };
		const auto dur = [&i](C y_) { return duration(i, curve::constant<U, C>(y_)); };

		auto [y, t, n] = root1d::newton(y0, tol, iter).solve(pv, dur);

		return y;
	}

	// Constant spread for which the present value of the instrument equals price.
	template<class IU, class IC, class T, class F>
	inline F oas(instrument::iterable<IU, IC> i, const curve::interface<T, F>& f, F p,
		F s0 = 0, F tol = math::sqrt_epsilon<F>, int iter = 100)
	{
		const auto pv = [p,&i,&f](F s_) { return present(i, f + curve::constant<T,F>(s_)) - p; };
		const auto dur = [&i,&f](F s_) { return duration(i, f + curve::constant<T, F>(s_)); };

		auto [s, t, n] = root1d::newton(s0, tol, iter).solve(pv, dur);

		return s;
	}

#ifdef _DEBUG
	template<class X = double>
	inline int yield_test()
	{
		using fms::iterable::array;

		X eps = X(1e-4); // math::sqrt_epsilon<X>;
		X y0 = X(0.03);
		X d1 = std::exp(-y0);
		X c0 = (1 - d1 * d1) / (d1 + d1 * d1);
		X u[] = { 1,2 };
		X c[] = { c0, 1 + c0 };
		// 1 = c0 exponential(-y0) + (1 + c0) exponential(-2 y0)
		const auto i = instrument::iterable(array(u), array(c));
		{
			X pv = present(i, curve::constant<X, X>(y0));
			assert(std::fabs(pv - 1) <= eps);
			X _pv = present(i, curve::constant<X, X>(y0 - eps));
			X pv_ = present(i, curve::constant<X, X>(y0 + eps));

			X dur = duration(i, curve::constant<X, X>(y0));
			X dur_ = (pv_ - _pv) / (2 * eps);
			assert(std::fabs(dur - dur_) <= eps);

			X cvx = convexity(i, curve::constant<X, X>(y0));
			X cvx_ = (pv_ - 2 * pv + _pv) / (eps * eps);
			assert(fabs(cvx - cvx_) < eps);
		}
		{
			X pv = present(i, curve::translate(curve::constant<X, X>(y0), X(0.5)));
			assert(std::fabs(pv - std::exp(y0*0.5)) <= eps);
		}
		{
			X y = yield(i, X(1));
			assert(std::fabs(y - y0) <= eps);
		}
		{
			X s = X(0.01);
			X pv = present(i, curve::constant<X, X>(y0 + s));
			X y = oas(i, curve::constant<X, X>(y0), pv, s);
			assert(std::fabs(pv - present(i, curve::constant<X, X>(y0 + y))) <= eps);
		}
		{
			X r = X(0.05);
			X y2 = compound_yield(r, 2);
			X r2 = continuous_rate(y2, 2);
			assert(std::fabs(r - r2) <= math::epsilon<X>);
		}

		return 0;
	}

#endif // _DEBUG

} // namespace tmx::value
