// tmx_value.h - present value, duration
#pragma once
#include <cmath>
#include <limits>
#include "tmx_instrument.h"
#include "tmx_pwflat_view.h"
#include "tmx_root1d.h"

namespace tmx::value {

	// Value of discounted cash flows.
	template<class U, class C, class T, class F>
	constexpr C present(size_t m, const U* u, const C* c, size_t n, const T* t, const F* f,
		F _f = std::numeric_limits<F>::quiet_NaN())
	{
		C pv = 0;

		for (size_t i = 0; i < m; ++i) {
			pv += c[i] * pwflat::discount(u[i], n, t, f, _f);
		}

		return pv;
	}
	// Value of discounted cash flows at t.
	template<class U, class C, class T, class F>
	constexpr C present(T t0, instrument_view<U, C> i, pwflat::curve_view<T, F> f)
	{
	}

	// Derivative of present value with respect to a parallel shift.
	template<class U, class C, class T, class F>
	constexpr C duration(size_t m, const U* u, const C* c, size_t n, const T* t, const F* f,
		F _f = std::numeric_limits<F>::quiet_NaN())
	{
		C dur = 0;

		for (size_t i = 0; i < m; ++i) {
			dur -= u[i] * c[i] * pwflat::discount(u[i], n, t, f, _f);
		}

		return dur;
	}

	// Second derivative of present value with respect to a parallel shift.
	template<class U, class C, class T, class F>
	constexpr C convexity(size_t m, const U* u, const C* c, size_t n, const T* t, const F* f,
		F _f = std::numeric_limits<F>::quiet_NaN())
	{
		C cnv = 0;

		for (size_t i = 0; i < m; ++i) {
			cnv += u[i] * u[i] * c[i] * pwflat::discount(u[i], n, t, f, _f);
		}

		return cnv;
	}

	// Constant forward rate matching price.
	template<class U, class C>
	inline C yield(const C p, size_t m, const U* u, const C* c, 
		C y = 0.01, C tol = std::sqrt(std::numeric_limits<C>::epsilon()), int iter = 100)
	{
		const auto pv = [=](C y_) { return present(m, u, c, 0, u, c, y_) - p; };
		const auto dur = [=](C y_) { return duration(m, u, c, 0, u, c, y_); };
		
		return newton::solve(pv, dur, y, tol, iter);
	}

	// Convert between continuous and compounded rate using (1 + y/n)^n = e^r
	template<class X>
	inline X compound_yield(X r, unsigned n)
	{
		return X(n*(std::pow(std::exp(r), 1. / n) - 1));
	}
	template<class X>
	inline X continuous_yield(X y, unsigned n)
	{
		return X(std::log(std::pow(1 + y/n, n)));
	}

#ifdef _DEBUG

	template<class X = double>
	inline int yield_test()
	{
		X eps = std::sqrt(std::numeric_limits<X>::epsilon());
		X y0 = X(0.03);
		X d1 = std::exp(-y0);
		X c0 = (1 - d1 * d1) / (d1 + d1 * d1);
		X u[] = { 0,1,2 };
		X c[] = { -1, c0, 1 + c0 };

		{
			X y = yield(X(0), 3, u, c);
			ensure(std::fabs(y - y0) <= eps);
		}
		{
			X y = yield(X(0), 3, u, c, y0);
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
