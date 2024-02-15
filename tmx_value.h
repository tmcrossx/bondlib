// tmx_value.h - present value, duration, convexity, yield
#pragma once
#include <cmath>
#include <limits>
#include "tmx_instrument.h"
#include "tmx_curve.h"
#include "tmx_root1d.h"

namespace tmx::value {

	// Convert between continuous and compounded rate using (1 + y/n)^n = e^r
	template<class X>
	inline X compound_yield(X r, unsigned n)
	{
		return X(n * (std::pow(std::exp(r), 1. / n) - 1));
	}
	template<class X>
	inline X continuous_yield(X y, unsigned n)
	{
		return X(std::log(std::pow(1 + y / n, n)));
	}

	// Present value at t of future discounted cash flows.
	template<class U, class C, class T, class F>
	constexpr C present(instrument::base<U, C>& i, const curve::base<T, F>& f, T t = 0)
	{
		C pv = 0;

		auto uc = i.iterable();
		while (uc) {
			const auto [u, c] = *uc;
			pv += c * f.discount(u, t);
			++uc;
		}

		return pv;
	}

	// Derivative of present value with respect to a parallel shift.
	template<class U, class C, class T, class F>
	constexpr C duration(instrument::base<U, C>& i, const curve::base<T, F>& f, T t = 0)
	{
		C dur = 0;

		while (i) {
			const auto [u, c] = *i;
			dur -= (u - t) * c * f.discount(u, t);
			++i;
		}

		return dur;
	}

	// Second derivative of present value with respect to a parallel shift.
	template<class U, class C, class T, class F>
	constexpr C convexity(instrument::base<U, C>& i, const curve::base<T, F>& f, T t = 0)
	{
		C cnv = 0;

		while (i) {
			const auto [u, c] = *i;
			cnv += (u - t) * (u - t) * c * f.discount(u, t);
			++i;
		}

		return cnv;
	}

	// Constant forward rate matching price p at t.
	template<class U, class C>
	inline C yield(instrument::base<U, C>& i, const C p = 0, U t = 0,
		C y = 0.01, C tol = math::sqrt_epsilon<C>, int iter = 100)
	{
		const auto pv = [=](C y_) { return present(i, curve::constant<U, C>(y_), t) - p; };
		const auto dur = [=](C y_) { return duration(i, curve::constant<U, C>(y_), t); };

		return root1d::newton(y, tol, iter).solve(pv, dur);
	}

	// TODO: (Tianxin)
	// Find constant spread so that the present value of the instrument equals price.
	template<class U, class C, class T, class F>
	inline F oas(F p, instrument::base<U, C>& i, const curve::base<T, F>& f,
		F s = 0, F tol = math::sqrt_epsilon<F>, int iter = 100)
	{
		//const auto pv = [=](C y_) { return present(i, curve::constant<U, C>(y_), t) - p; };
		//const auto dur = [=](C y_) { return duration(i, curve::constant<U, C>(y_), t); };

		//return root1d::newton(y, tol, iter).solve(pv, dur);
		// Newton solve for s with p = pv(i, f + s).!!!

		return s;
	}
	// test!!!


#ifdef _DEBUG
#if 0
	template<class X = double>
	inline int yield_test()
	{
		X eps = std::sqrt(std::numeric_limits<X>::epsilon());
		X y0 = X(0.03);
		X d1 = std::exp(-y0);
		X c0 = (1 - d1 * d1) / (d1 + d1 * d1);
		X u[] = { 1,2 };
		X c[] = { c0, 1 + c0 };
		// 1 = c0 exp(-y0) + (1 + c0) exp(-2 y0)
		const auto i = instrument::view(std::span(u), std::span(c));

		{
			X y = yield(i, X(1));
			ensure(std::fabs(y - y0) <= eps);
		}
		// TODO: test at forward values of time.
		{
			// 1 = c0 exp(-y0 * .5) + (1 + c0) exp(-y0 * 1.5)
			// yield(i, X(1), U(0.5))???
		}
		{
			X y = yield(i, X(1), X(0), y0);
			ensure(std::fabs(y - y0) <= eps);
		}
		{
			X r = X(0.05);
			X y2 = compound_yield(r, 2);
			X r2 = continuous_yield(y2, 2);
			ensure(std::fabs(r - r2) <= math::epsilon<X>);
		}

		return 0;
	}

#endif // _DEBUG
#endif
} // namespace tmx::value
