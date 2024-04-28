// tmx_valuation.h - present value, duration, convexity, yield, oas
#pragma once
#include <cmath>
#include "tmx_instrument.h"
#include "tmx_curve.h"
#include "tmx_root1d.h"

using namespace fms::iterable;

namespace tmx::valuation {

	// Convert between continuous and compounded rate using (1 + y/n)^n = e^r
	template<class X>
	inline X continuous_yield(X y, unsigned n)
	{
		return X(std::log(std::pow(1 + y / n, n)));
	}
	template<class X>
	inline X compound_yield(X r, unsigned n)
	{
		return X(n*(std::exp(r/n) - 1));
	}

	// Present value at t of a zero coupon bond with cash flow c at time u.
	template<class U, class C, class T, class F>
	constexpr auto present(const instrument::cash_flow<U, C>& uc, const curve::interface<T, F>& f, T t = 0)
	{
		return uc.c * f.discount(uc.u, t);
	}
#ifdef _DEBUG
	//static_assert(present<int,int,int,int>(instrument::cash_flow(1, 1), curve::constant(0)) == 1);
#endif // _DEBUG

	// Present value at t of future discounted cash flows.
	template<fms::iterable::input I, class T, class F>
	constexpr auto present(const I& i, const curve::interface<T, F>& f, T t = 0)
	{
		return sum(apply([&f, t](const auto& uc) { return present(uc, f, t); }, i));
	}
#ifdef _DEBUG
	//static_assert(present<int,int,int,int>(instrument::zero_coupon_bond(1, 2), curve::constant(0)) == 2);
#endif // _DEBUG

	// Derivative of present value with respect to a parallel shift.
	template<input I, class T, class F>
	constexpr auto duration(const I& i, const curve::interface<T, F>& f, T t = 0)
	{
		return sum(apply([&f, t](const auto& uc) { return -(uc.u - t) * present(uc, f, t); }, i));
	}

	// Second derivative of present value with respect to a parallel shift.
	template<input I, class T, class F>
	constexpr auto convexity(const I& i, const curve::interface<T, F>& f, T t = 0)
	{
		return sum(apply([&f, t](const auto& uc) { return (uc.u - t) * (uc.u - t) * present(uc, f, t); }, i));
	}

	// Constant yield matching price p at t.
	template<input I, class U = typename I::value_type::time_type, class C = typename I::value_type::cash_type>
	inline C yield(const I& i, const C p = 0, U t = 0,
		C y = 0.01, C tol = math::sqrt_epsilon<C>, int iter = 100)
	{
		const auto pv = [p,t,&i](C y_) { return present(i, curve::constant<U, C>(y_), t) - p; };
		const auto dur = [p,t,&i](C y_) { return duration(i, curve::constant<U, C>(y_), t); };

		return root1d::newton(y, tol, iter).solve(pv, dur);
	}

	// Constant spread for which the present value of the instrument equals price.
	template<input I, class T, class F>
	inline F oas(F p, I i, const curve::interface<T, F>& f,
		F s = 0, T t = 0, F tol = math::sqrt_epsilon<F>, int iter = 100)
	{
		const auto pv = [=](F s_) { return present(i, f + s_, t) - p; };
		const auto dur = [=](F s_) { return duration(i, f + s_, t); };

		return root1d::newton(s, tol, iter).solve(pv, dur);
	}

#ifdef _DEBUG
	template<class X = double>
	inline int yield_test()
	{
		using fms::iterable::concatenate;
		using instrument::zero_coupon_bond;

		X eps = math::sqrt_epsilon<X>;
		X y0 = X(0.03);
		X d1 = std::exp(-y0);
		X c0 = (1 - d1 * d1) / (d1 + d1 * d1);
		X u[] = { 1,2 };
		X c[] = { c0, 1 + c0 };
		// 1 = c0 exp(-y0) + (1 + c0) exp(-2 y0)
		const auto i = concatenate(zero_coupon_bond(u[0], c[0]), zero_coupon_bond(u[1], c[1]));
		X pv = present(i, curve::constant<X, X>(y0));
		assert(std::fabs(pv - 1) <= eps);

		{
			X y = yield(i, X(1));
			assert(std::fabs(y - y0) <= eps);
		}
		// TODO: test at forward values of time.
		{
			// 1 = c0 exp(-y0 * .5) + (1 + c0) exp(-y0 * 1.5)
			// yield(i, X(1), U(0.5))???
		}
		{
			//X y = yield(i, X(1), X(0), y0);
			//ensure(std::fabs(y - y0) <= eps);
		}
		{
			X r = X(0.05);
			X y2 = compound_yield(r, 2);
			X r2 = continuous_yield(y2, 2);
			assert(std::fabs(r - r2) <= math::epsilon<X>);
		}

		return 0;
	}

#endif // _DEBUG

} // namespace tmx::value
