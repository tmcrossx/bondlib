// tmx_value.h - present value, duration, convexity
#pragma once
#include <cmath>
#include <limits>
#include "tmx_instrument.h"
#include "tmx_pwflat.h"
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

	// Value at t of future discounted cash flows.
	template<class U, class C, class T, class F>
	constexpr C present(const instrument<U,C>& i, const pwflat::curve<T,F>& f, T t = 0)
	{
		const auto u = i.time();
		const auto c = i.cash();

		C pv = 0;
		size_t j = u.offset(t);
		for (; j < u.size(); ++j) {
			pv += c[j] * f.discount(u[j], t);
		}

		return pv;
	}

	// Derivative of present value with respect to a parallel shift.
	template<class U, class C, class T, class F>
	constexpr C duration(const instrument<U, C>& i, const pwflat::curve<T, F>& f, T t = 0)
	{
		const auto u = i.time();
		const auto c = i.cash();

		C dur = 0;
		size_t j = u.offset(t);
		for (; j < u.size(); ++j) {
			dur -= (u[j] - t) * c[j] * f.discount(u[j], t);
		}

		return dur;
	}

	// Second derivative of present value with respect to a parallel shift.
	template<class U, class C, class T, class F>
	constexpr C convexity(const instrument<U, C>& i, const pwflat::curve<T, F>& f, T t = 0)
	{
		const auto u = i.time();
		const auto c = i.cash();

		C cnv = 0;
		size_t j = u.offset(t);
		for (; j < u.size(); ++j) {
			cnv += (u[j] - t) * (u[j] - t) * c[j] * f.discount(u[j], t);
		}

		return cnv;
	}

	// Constant forward rate matching price.
	template<class U, class C>
	inline C yield(const C p, const instrument<U,C>& i, 
		C y = 0.01, C tol = std::sqrt(std::numeric_limits<C>::epsilon()), int iter = 100)
	{
		const auto pv = [&](C y_) { return present(i, pwflat::curve_view<U,C>(y_)) - p; };
		const auto dur = [&](C y_) { return duration(i, pwflat::curve_view<U, C>(y_)); };
		
		return newton::solve(pv, dur, y, tol, iter);
	}

#ifdef _DEBUG

	template<class X = double>
	inline int yield_test()
	{
		X eps = std::sqrt(std::numeric_limits<X>::epsilon());
		X y0 = X(0.03);
		X d1 = std::exp(-y0);
		X c0 = (1 - d1 * d1) / (d1 + d1 * d1);
		X u[] = { 1,2 };
		X c[] = { c0, 1 + c0 };
		const auto i = instrument_view(view(u), view(c));

		{
			X y = yield(X(1), i);
			ensure(std::fabs(y - y0) <= eps);
		}
		{
			X y = yield(X(1), i, y0);
			ensure(std::fabs(y - y0) <= eps);
		}
		{
			X r = X(0.05);
			X y2 = compound_yield(r, 2);
			X r2 = continuous_yield(y2, 2);
			ensure(std::fabs(r - r2) <= std::numeric_limits<X>::epsilon());
		}

		return 0;
	}

#endif // _DEBUG

}
