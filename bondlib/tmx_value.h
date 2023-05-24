// tmx_value.h - present value, duration
#pragma once
#include <cmath>
#include <limits>

namespace tmx::value {

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
	constexpr C duration(size_t m, const U* u, const C* c, const D& d)
	{
		C dur = 0;

		for (size_t i = 0; i < m; ++i) {
			dur += u[i] * c[i] * d(u[i]);
		}

		return dur;
	}

	// Constant forward rate matching price.
	template<class U, class C>
	constexpr C yield(const C p, size_t m, const U* u, const C* c, 
		C y = 0, C eps = std::sqrt(std::numeric_limits<C>::epsilon()))
	{
		C y_ = y + 2 * eps;

		while (std::fabs(y_ - y) > eps) {
			C p_ = present(m, u, c, [y](U t) { return exp(-y * t); }) - p;
			C dp_ = duration(m, u, c, [y](U t) { return exp(-y * t); });
			C y_ = y - p_ / dp_;
			std::swap(y_, y);
		}

		return y;
	}

#ifdef _DEBUG

	inline int test_value_yield()
	{
		constexpr double u[] = { 0,1,2 };
		constexpr double c[] = { -1, 0.03, 1.03 };
		{
			double y = yield(0., 3, u, c);
			y = y;
		}
		return 0;
	}
#endif // _DEBUG

}
