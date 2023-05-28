// tmx_value.h - present value, duration
#pragma once
#include <cmath>
#include <limits>

namespace tmx::value {

	// Value of discounted cash flows.
	template<class U, class C, class D>
	constexpr C present(size_t m, const U* u, const C* c, D d)
	{
		C pv = 0;

		for (size_t i = 0; i < m; ++i) {
			pv += c[i] * d(u[i]);
		}

		return pv;
	}
	template<class U, class C, class D>
	constexpr C present(const std::span<U>& u, const std::span<C>& c, D d)
	{
		if (u.size() != c.size()) {
			return std::numeric_limits<C>::quiet_NaN();
		}

		return present(u.size(), u.data(), c.data(), d);
	}


	// Derivative of present value with respect to a parallel shift.
	template<class U, class C, class D>
	constexpr C duration(size_t m, const U* u, const C* c, const D& d)
	{
		C dur = 0;

		for (size_t i = 0; i < m; ++i) {
			dur -= u[i] * c[i] * d(u[i]);
		}

		return dur;
	}
	template<class U, class C, class D>
	constexpr C duration(const std::span<U>& u, const std::span<C>& c, D d)
	{
		if (u.size() != c.size()) {
			return std::numeric_limits<C>::quiet_NaN();
		}

		return duration(u.size(), u.data(), c.data(), d);
	}

	// Constant forward rate matching price.
	template<class U, class C>
	inline C yield(const C p, size_t m, const U* u, const C* c, 
		C y = 0, C eps = std::sqrt(std::numeric_limits<C>::epsilon()))
	{
		C y_ = y + 2 * eps; // at least one loop

		while (std::fabs(y_ - y) > eps) {
			C p_ = present(m, u, c, [y](U t) { return std::exp(-y * t); }) - p;
			C dp_ = duration(m, u, c, [y](U t) { return std::exp(-y * t); });
			y_ = y - p_ / dp_; // Newton-Raphson
			std::swap(y_, y);
		}

		return y;
	}
	template<class U, class C, class D>
	constexpr C yield(const C p, const std::span<U>& u, const std::span<C>& c, D d)
	{
		if (u.size() != c.size()) {
			return std::numeric_limits<C>::quiet_NaN();
		}

		return yield(p, u.size(), u.data(), c.data(), d);
	}

	// Convert from continuously compounded rate using (1 + y/n)^n = e^r
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
	inline int value_yield_test()
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
